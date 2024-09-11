#include <csignal>
#include <iostream>
#include "server.h"

// Constructor
Server::Server(int port, std::function<void(Packet&)> callback, ISocket* socketInterface) {
    setPort(port);
    setReceiveDataCallback(callback);
    setSocketInterface(socketInterface);
    running = false;
}

// Initializes the listening socket
ErrorCode Server::startConnection()
{
    // Create socket TCP
    serverSocket = socketInterface->socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
        return ErrorCode::SOCKET_FAILED;

    // Setting the socket to allow reuse of address and port
    int opt = 1;
    int setSockOptRes = socketInterface->setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    if (setSockOptRes) {
        socketInterface->close(serverSocket);
        return ErrorCode::SOCKET_FAILED;
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    int bindRes = socketInterface->bind(serverSocket, (struct sockaddr *)&address, sizeof(address));
    if (bindRes < 0) {
        socketInterface->close(serverSocket);
        return ErrorCode::BIND_FAILED;
    }

    int lisRes = socketInterface->listen(serverSocket, 5);
    if (lisRes < 0) {
        socketInterface->close(serverSocket);
        return ErrorCode::LISTEN_FAILED;
    }
    
    running = true;
    mainThread = std::thread(&Server::startThread, this);
    mainThread.detach();

    return ErrorCode::SUCCESS;
}

// Starts listening for connection requests
void Server::startThread()
{
    while (running) {
        int clientSocket = socketInterface->accept(serverSocket, nullptr, nullptr);
        if (!clientSocket)
            continue;
        
        if(clientSocket<0){
            stopServer();
            return;
        }
        // Opens a new thread for handleClient - listening to messages from the process
        {
            std::lock_guard<std::mutex> lock(threadMutex);
            clientThreads.emplace_back(&Server::handleClient, this, clientSocket);
        } 
    }
}

// Closes the sockets and the threads
void Server::stopServer()
{
    if(!running)
        return;
        
    running = false;
    socketInterface->close(serverSocket);
    {
        std::lock_guard<std::mutex> lock(socketMutex);
        for (int sock : sockets)
            socketInterface->close(sock);
        sockets.clear();
    }
    {
        std::lock_guard<std::mutex> lock(threadMutex);
        for (auto &th : clientThreads)
        if (th.joinable())
            th.join();
    }
}

// Runs in a thread for each process - waits for a message and forwards it to the manager
void Server::handleClient(int clientSocket)
{
    Packet packet;
    int valread = socketInterface->recv(clientSocket, &packet, sizeof(Packet), 0);

    //implement according to CAN bus
    if (valread <= 0)
        return;
    
    uint32_t clientID = packet.header.SrcID;
    if(!isValidId(clientID))
        return;

    {
        std::lock_guard<std::mutex> lock(IDMapMutex);
        clientIDMap[clientSocket] = clientID;
    }

    {
        std::lock_guard<std::mutex> lock(socketMutex);
        sockets.push_back(clientSocket);
    }

    while (true) {
        int valread = socketInterface->recv(clientSocket, &packet, sizeof(Packet), 0);
        if (!valread)
            continue;

        if(valread<0)
            break;
        
        receiveDataCallback(packet);
    }

    {
    // If the process is no longer connected
    std::lock_guard<std::mutex> lock(socketMutex);
    auto it = std::find(sockets.begin(), sockets.end(), clientSocket);
    socketInterface->close(*it);
    if (it != sockets.end())
        sockets.erase(it);
    }
    {
        std::lock_guard<std::mutex> lock(IDMapMutex);
        clientIDMap.erase(clientSocket);
    }
}

// Implementation according to the CAN BUS
bool Server::isValidId(uint32_t id)
{
    if(id==0)
        return false;
        
    //implement id exist already
    return true;
}

// Returns the sockets ID
int Server::getClientSocketByID(uint32_t destID)
{
    std::lock_guard<std::mutex> lock(IDMapMutex);
    for (const auto &client : clientIDMap)
        if (client.second == destID)
            return client.first;

    return -1;
}

// Sends the message to destination
ErrorCode Server::sendDestination(const Packet &packet)
{
    int targetSocket = getClientSocketByID(packet.header.DestID);
    if (targetSocket == -1)
        return ErrorCode::INVALID_CLIENT_ID;
    
    ssize_t bytesSent = socketInterface->send(targetSocket, &packet, sizeof(Packet), 0);
    if (!bytesSent)
        return ErrorCode::SEND_FAILED;

    if (bytesSent<0){
        //closeConnection();
        return ErrorCode::CONNECTION_FAILED;
    }
    
    return ErrorCode::SUCCESS;
}

// Sends the message to all connected processes - broadcast
ErrorCode Server::sendBroadcast(const Packet &packet)
{
    std::lock_guard<std::mutex> lock(socketMutex);
    for (int sock : sockets) {
        ssize_t bytesSent = socketInterface->send(sock, &packet, sizeof(Packet), 0);
        if (bytesSent < sizeof(Packet))
            return ErrorCode::SEND_FAILED;
        if (bytesSent<0){
            //closeConnection();
            return ErrorCode::CONNECTION_FAILED;
        }
    }

    return ErrorCode::SUCCESS;
}

// Sets the server's port number, throws an exception if the port is invalid.
void Server::setPort(int port) {
    if (port <= 0 || port > 65535)
        throw std::invalid_argument("Invalid port number: Port must be between 1 and 65535.");

    this->port = port;
}

// Sets the callback for receiving data, throws an exception if the callback is null.
void Server::setReceiveDataCallback(std::function<void(Packet&)> callback) {
    if (!callback) {
        throw std::invalid_argument("Invalid callback function: callback cannot be null.");
    }
    this->receiveDataCallback = callback;
}

// Sets the socket interface, throws an exception if the socketInterface is null.
void Server::setSocketInterface(ISocket* socketInterface) {
    if (socketInterface == nullptr) {
        throw std::invalid_argument("Invalid socket interface: socketInterface cannot be null.");
    }
    this->socketInterface = socketInterface;
}

// For testing
int Server::getServerSocket()
{
    return serverSocket;
}

int Server::isRunning()
{
    return running;
}

std::vector<int>* Server::getSockets()
{
    return &sockets;
}

std::mutex* Server::getSocketMutex()
{
    return &socketMutex;
}

std::mutex* Server::getIDMapMutex()
{
    return &IDMapMutex;
}

std::map<int, uint32_t>* Server::getClientIDMap()
{
    return &clientIDMap;
}

void Server::testHandleClient(int clientSocket)
{
    handleClient(clientSocket);
}

int Server::testGetClientSocketByID(uint32_t destID)
{
    return getClientSocketByID(destID);
}

// Destructor
Server::~Server()
{
    stopServer();
    delete socketInterface;
}