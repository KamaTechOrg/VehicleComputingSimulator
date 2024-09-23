#include "../include/client_connection.h"

// Constructor
ClientConnection::ClientConnection(std::function<void(const Packet &)> callback, ISocket* socketInterface): connected(false)
{
        setCallback(callback);
#ifdef ESP32
    setSocketInterface(new ESP32Socket("Wokwi-GUEST", ""));
#else
    setSocketInterface(new RealSocket());
#endif
}

// Function to load the server configuration from a JSON file
ErrorCode ClientConnection::loadServerConfig(const std::string& filePath)
{
#ifdef ESP32
    serverPort = PORT;
    serverIP = IP;
#else
    // Open the JSON file
    std::ifstream file(filePath);
    if (!file.is_open())
        return ErrorCode::FILE_OPEN_FAILED;

    // Parse the JSON file
    nlohmann::json jsonDoc;
    try {
        file >> jsonDoc;
    } catch (const nlohmann::json::parse_error& e) {
        return ErrorCode::JSON_PARSE_FAILED;
    }
        
    // Check if the necessary keys are present
    if (jsonDoc.contains("server") && jsonDoc["server"].contains("ip") && jsonDoc["server"].contains("port")) {
        serverIP = jsonDoc["server"]["ip"].get<std::string>().c_str();
        serverPort = jsonDoc["server"]["port"].get<uint16_t>();
    } else {
        return ErrorCode::MISSING_KEYS;
    }

    file.close();
#endif
    return ErrorCode::SUCCESS;
}

// Requesting a connection to the server
ErrorCode ClientConnection::connectToServer(uint32_t processID)
{
    ErrorCode result = loadServerConfig("..//config.json");
    if (result != ErrorCode::SUCCESS)
        return result;
    
    clientSocket = socketInterface->socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        return ErrorCode::SOCKET_FAILED;
    }

    servAddress.sin_family = AF_INET;
    servAddress.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverIP, &servAddress.sin_addr);

    int connectRes = socketInterface->connect(clientSocket, (struct sockaddr *)&servAddress, sizeof(servAddress));
    if (connectRes < 0) {
        socketInterface->close(clientSocket);
        return ErrorCode::CONNECTION_FAILED;
    }

    Packet packet(processID);
    ssize_t bytesSent = socketInterface->send(clientSocket, &packet, sizeof(Packet), 0);
    if (bytesSent < sizeof(Packet)) {
        socketInterface->close(clientSocket);
        return ErrorCode::SEND_FAILED;
    }

    connected.exchange(true);

    ssize_t bytesRecv = socketInterface->recv(clientSocket, &packet, sizeof(Packet), 0);
    if (bytesSent < sizeof(Packet)) {
        socketInterface->close(clientSocket);
        return ErrorCode::RECEIVE_FAILED;
    }

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
ErrorCode ClientConnection::sendPacket(const Packet &packet)
{
    //If send executed before start
    if (!connected.load())
        return ErrorCode::CONNECTION_FAILED;
        
    ssize_t bytesSent = socketInterface->send(clientSocket, &packet, sizeof(Packet), 0);
    if (bytesSent==0) {
        closeConnection();
        return ErrorCode::CONNECTION_FAILED;
    }
    if (bytesSent<0)
        return ErrorCode::SEND_FAILED;
        
    return ErrorCode::SUCCESS;
}

// Waits for a message and forwards it to Communication
void ClientConnection::receivePacket() 
{
    while (connected.load()) {
        Packet packet;
        int valread = socketInterface->recv(clientSocket, &packet, sizeof(Packet), 0);
        if (valread==0)
            break;

        if (valread<0)
            continue;

        passPacketCom(packet);
    }

    closeConnection();
}

// Closes the connection
ErrorCode ClientConnection::closeConnection()
{
    if (connected.load()) {
        int socketInterfaceRes = socketInterface->close(clientSocket);
        if(socketInterfaceRes < 0)
            return ErrorCode::CLOSE_FAILED;
        connected.exchange(false);

#ifdef ESP32
    if (receiveTaskHandle != nullptr) {
        vTaskDelete(receiveTaskHandle); // Close the FreeRTOS task if it's running
        receiveTaskHandle = nullptr;
    }
#else
    RealSocket::log.cleanUp();
#endif
    }
    return ErrorCode::SUCCESS;  
}

// Setter for passPacketCom
void ClientConnection::setCallback(const std::function<void(const Packet&)> callback) {
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
int ClientConnection::getClientSocket() const 
{
    return clientSocket;
}

int ClientConnection::isConnected() const 
{
    return connected.load();
}

bool ClientConnection::isReceiveThreadRunning() const 
{
#ifdef ESP32
    return receiveTaskHandle != nullptr;
#else
    return false;
#endif
}

//Destructor
ClientConnection::~ClientConnection()
{
    closeConnection();
    delete socketInterface;
}