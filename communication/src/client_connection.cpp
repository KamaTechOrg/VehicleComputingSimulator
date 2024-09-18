#include "client_connection.h"

// Constructor
ClientConnection::ClientConnection(std::function<void(Packet &)> callback, ISocket* socketInterface)
    : connected(false)
{
    setCallback(callback);
#ifdef ESP32
    setSocketInterface(new ESP32Socket("Wokwi-GUEST", ""));
#else
    setSocketInterface(new RealSocket());
#endif
}

// Requesting a connection to the server
ErrorCode ClientConnection::connectToServer(int id)
{
    clientSocket = socketInterface->socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        return ErrorCode::SOCKET_FAILED;
    }

    servAddress.sin_family = AF_INET;
    servAddress.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &servAddress.sin_addr);

    int connectRes = socketInterface->connect(clientSocket, (struct sockaddr *)&servAddress, sizeof(servAddress));
    if (connectRes < 0) {
        socketInterface->close(clientSocket);
        return ErrorCode::CONNECTION_FAILED;
    }

    Packet packet(id);
    ssize_t bytesSent = socketInterface->send(clientSocket, &packet, sizeof(Packet), 0);
    if (bytesSent < sizeof(Packet)) {
        socketInterface->close(clientSocket);
        return ErrorCode::SEND_FAILED;
    }
    
    connected = true;

#ifdef ESP32
    // Create a FreeRTOS task for receiving packets
    xTaskCreatePinnedToCore([](void* param) {
        static_cast<ClientConnection*>(param)->receivePacket();
    }, "ReceiveTask", 4096, this, 1, &receiveTaskHandle, 0);
#else
    // Create a standard C++ thread for receiving packets
    receiveThread = std::thread(&ClientConnection::receivePacket, this);
    receiveThread.detach();
#endif

    return ErrorCode::SUCCESS;
}

// Sends the packet to the manager-sync
ErrorCode ClientConnection::sendPacket(Packet &packet)
{
    // If send executed before start
    if (!connected)
        return ErrorCode::CONNECTION_FAILED;
        
    ssize_t bytesSent = socketInterface->send(clientSocket, &packet, sizeof(Packet), 0);
    if (bytesSent < 0) {
        closeConnection();
        return ErrorCode::SEND_FAILED;
    }
        
    return ErrorCode::SUCCESS;
}

// Waits for a message and forwards it to Communication
void ClientConnection::receivePacket()
{
    while (connected) {
        Packet packet;
        int valread = socketInterface->recv(clientSocket, &packet, sizeof(Packet), 0);
        if (!valread)
            continue;

        if(valread < 0)
            break;

        passPacketCom(packet);
    }

    closeConnection();
}

// Closes the connection
ErrorCode ClientConnection::closeConnection()
{
    connected = false;
    int socketInterfaceRes = socketInterface->close(clientSocket);
    if (socketInterfaceRes < 0)
        return ErrorCode::CLOSE_FAILED;

#ifdef ESP32
    if (receiveTaskHandle != nullptr) {
        vTaskDelete(receiveTaskHandle); // Close the FreeRTOS task if it's running
        receiveTaskHandle = nullptr;
    }
#endif

    return ErrorCode::SUCCESS;
}

// Setter for passPacketCom
void ClientConnection::setCallback(std::function<void(Packet&)> callback) {
    if (!callback)
        throw std::invalid_argument("Callback function cannot be null");
    
    passPacketCom = callback;
}

// Setter for socketInterface
void ClientConnection::setSocketInterface(ISocket* socketInterface) {
    if (!socketInterface)
        throw std::invalid_argument("Socket interface cannot be null");
    
    this->socketInterface = socketInterface;
}

// For testing
int ClientConnection::getClientSocket()
{
    return clientSocket;
}

int ClientConnection::isConnected()
{
    return connected;
}

bool ClientConnection::isReceiveThreadRunning()
{
#ifdef ESP32
    return receiveTaskHandle != nullptr;
#else
    return receiveThread.joinable();
#endif
}

// Destructor
ClientConnection::~ClientConnection()
{
    closeConnection();
    delete socketInterface;
}
