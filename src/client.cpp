#include "client.h"

// Constructor
Client::Client(uint32_t id, std::function<void(Packet &)> callback, ISocket* socketInterface)
    : id(id), passPacketCom(callback), connected(false), socketInterface(socketInterface){}

// Requesting a connection to the server
int Client::connectToServer()
{
    clientSocket = socketInterface->socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        Logger::generalLogMessage(std::to_string(id), LogLevel::ERROR, "failed to create a client socket");
        return -1;
    }
    Logger::generalLogMessage(std::to_string(id), LogLevel::INFO, "create a client socket: " + std::to_string(clientSocket));

    servAddress.sin_family = AF_INET;
    servAddress.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &servAddress.sin_addr);

    int connectRes = socketInterface->connect(clientSocket, (struct sockaddr *)&servAddress, sizeof(servAddress));
    if (connectRes < 0) {
        Logger::generalLogMessage(std::to_string(id), LogLevel::ERROR, "error to connect to server");
        return -1;
    }
    Logger::generalLogMessage(std::to_string(id), LogLevel::INFO, "connection succeed to server socket");

    Packet packet(id);
    ssize_t bytesSent = socketInterface->send(clientSocket, &packet, sizeof(Packet), 0);
    if (bytesSent < sizeof(Packet)) {
        Logger::packetLogMessage(std::to_string(id), LogLevel::ERROR, "sending id for init failed", packet);
        return -1;
    }
    Logger::packetLogMessage(std::to_string(id), LogLevel::INFO, "sending id for init succeed", packet);
    
    connected = true;
    receiveThread = std::thread(&Client::receivePacket, this);
    receiveThread.detach();
    return 0;
}

// Sends the packet to the manager-sync
int Client::sendPacket(Packet &packet)
{
    //If send executed before start
    if (!connected)
        return -1;
        
    ssize_t bytesSent = socketInterface->send(clientSocket, &packet, sizeof(Packet), 0);
    if (bytesSent < sizeof(Packet)) {
        Logger::packetLogMessage(std::to_string(id), LogLevel::ERROR, "sending failed", packet);
        return -1;
    }
    Logger::packetLogMessage(std::to_string(id), LogLevel::INFO, "sending succeed", packet);
    
    return 0;
}

// Waits for a message and forwards it to Communication
void Client::receivePacket()
{
    Packet packet;
    while (connected) {
        int valread = socketInterface->recv(clientSocket, &packet, sizeof(Packet), 0);
        if (valread <= 0) {
            Logger::generalLogMessage(std::to_string(id), LogLevel::ERROR, "Connection closed or error occurred in client socket number: " + std::to_string(clientSocket));
            break;
        }
        Logger::packetLogMessage(std::to_string(id), LogLevel::INFO, "received packet", packet);
        passPacketCom(packet);
    }
}

// Closes the connection
void Client::closeConnection()
{
    //implement - Notify the manager about disconnection
    connected = false;
    socketInterface->close(clientSocket);
    if(receiveThread.joinable())
        receiveThread.join();
    Logger::generalLogMessage(std::to_string(id), LogLevel::INFO, "close client socket number: " + std::to_string(clientSocket));
}

//For testing
int Client::getClientSocket()
{
    return clientSocket;
}

int Client::isConnected()
{
    return connected;
}

bool Client::isReceiveThreadRunning()
{
    return false;
}

//Destructor
Client::~Client()
{
    closeConnection();
    delete socketInterface;
}