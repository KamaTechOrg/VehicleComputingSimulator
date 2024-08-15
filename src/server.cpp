#include "server.h"

// Constructor
Server::Server(int port, std::function<void(void *)> callback, ISocket *socketInterface)
    : port(port), receiveDataCallback(callback), running(false), socketInterface(socketInterface) {}

// Initializes the listening socket
int Server::startConnection()
{
    // Create socket TCP
    serverSocket = socketInterface->socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        Logger::generalLogMessage("manager", LogLevel::ERROR, "failed to create a server socket");
        return -1;
    }

    // Setting the socket to allow reuse of address and port
    int opt = 1;
    int setSockOptRes = socketInterface->setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    if (setSockOptRes) {
        Logger::generalLogMessage("manager", LogLevel::ERROR, "failed to set a server socket");
        socketInterface->close(serverSocket);
        return -1;
    }
    Logger::generalLogMessage("manager", LogLevel::INFO, "create a server socket: "+std::to_string(serverSocket));
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    int bindRes = socketInterface->bind(serverSocket, (struct sockaddr *)&address, sizeof(address));
    if (bindRes < 0) {
        Logger::generalLogMessage("manager", LogLevel::ERROR, "failed to bind a server socket");
        socketInterface->close(serverSocket);
    }

    int lisRes = socketInterface->listen(serverSocket, 5);
    if (lisRes < 0) {
        Logger::generalLogMessage("manager", LogLevel::ERROR, "listening error occurred");
        socketInterface->close(serverSocket);
        return -1;
    }
    Logger::generalLogMessage("manager", LogLevel::INFO, "server running on port " + std::to_string(port));
    
    running = true;
    mainThread = std::thread(&Server::startThread, this);
    mainThread.detach();
    return 0;
}

// Starts listening for connection requests
void Server::startThread()
{
    while (running) {
        int clientSocket = socketInterface->accept(serverSocket, nullptr, nullptr);
        if (clientSocket < 0) {
            Logger::generalLogMessage("manager", LogLevel::ERROR, "accepting error occurred");
            return;
        }
        Logger::generalLogMessage("manager", LogLevel::INFO, "connection succeed to client socket number: " + std::to_string(clientSocket));
        
        // Opens a new thread for handleClient - listening to messages from the process
        clientThreads.emplace_back(&Server::handleClient, this, clientSocket);
    }
}

// Implementation according to the CAN BUS
bool Server::isValidId(uint32_t id)
{
    return true;
}

// Closes the sockets and the threads
void Server::stopServer()
{
    running = false;
    Logger::generalLogMessage("manager", LogLevel::INFO, "close manager socket number: " + std::to_string(serverSocket));
    socketInterface->close(serverSocket);

    std::lock_guard<std::mutex> lock(socketMutex);
    for (int sock : sockets) {
        Logger::generalLogMessage("manager", LogLevel::INFO, "close client socket number: " + std::to_string(sock));
        socketInterface->close(sock);
    }

    for (auto &th : clientThreads) {
        if (th.joinable())
            th.join();
    }
}

// Runs in a thread for each process - waits for a message and forwards it to the manager
void Server::handleClient(int clientSocket)
{
    Packet packet;
    int valread = socketInterface->recv(clientSocket, &packet, sizeof(Packet), 0);
    if (valread <= 0) {
        Logger::generalLogMessage("manager", LogLevel::ERROR, "Connection closed or error occurred");
        return;
    }
    Logger::packetLogMessage("manager", LogLevel::INFO, "received ID for init", packet);
    
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
        if (valread <= 0) {
            Logger::generalLogMessage("manager", LogLevel::ERROR, "Connection closed or error occurred in client socket number: " + std::to_string(clientSocket));
            break;
        }
        Logger::packetLogMessage("manager", LogLevel::INFO, "received packet", packet);
        receiveDataCallback(&packet);
    }
    
    // If the process is no longer connected
    std::lock_guard<std::mutex> lock(socketMutex);
    auto it = std::find(sockets.begin(), sockets.end(), clientSocket);
    if (it != sockets.end())
        sockets.erase(it);
}

// Returns the sockets ID
int Server::getClientSocketByID(uint32_t destID)
{
    std::lock_guard<std::mutex> lock(IDMapMutex);
    for (const auto &client : clientIDMap) {
        if (client.second == destID)
            return client.first;
    }
    return -1; // If not found
}

// Sends the message to destination
int Server::sendDestination(const Packet &packet)
{
    int targetSocket = getClientSocketByID(packet.header.DestID);
    if (targetSocket == -1)
        return -1;
    
    ssize_t bytesSent = socketInterface->send(targetSocket, &packet, sizeof(Packet), 0);
    if (bytesSent < sizeof(Packet)) {
        Logger::packetLogMessage("manager", LogLevel::ERROR, "sending failed", packet);
        return -1;
    }
    Logger::packetLogMessage("manager", LogLevel::INFO, "sending succeed", packet);
    
    return 0;
}

// Sends the message to all connected processes - broadcast
int Server::sendBroadcast(const Packet &packet)
{
    std::lock_guard<std::mutex> lock(socketMutex);
    for (int sock : sockets) {
        ssize_t bytesSent = socketInterface->send(sock, &packet, sizeof(Packet), 0);
        if (bytesSent < sizeof(Packet)) {
            Logger::packetLogMessage("manager", LogLevel::ERROR, "sending failed", packet);
            return -1;
        }
        Logger::packetLogMessage("manager", LogLevel::INFO, "sending succeed", packet);
    }

    return 0;
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

// Destructor
Server::~Server()
{
    stopServer();
    delete socketInterface;
}
