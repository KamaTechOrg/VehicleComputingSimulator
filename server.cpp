#include "server.hpp"

Server::Server(int port, std::function<void(void *)> callback) : port(port), receiveData(callback), running(false)
{
    // create socket TCP
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        Logger::generalLogMessage("manager", LogLevel::ERROR, "failed to create a server socket");
        return;
    }

    // Setting the socket to allow reuse of address and port
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        Logger::generalLogMessage("manager", LogLevel::ERROR, "failed to set a server socket");
        close(serverSocket);
        return;
    }
    Logger::generalLogMessage("manager", LogLevel::INFO, "create a server socket");

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        Logger::generalLogMessage("manager", LogLevel::ERROR, "failed to bind a server socket");
        close(serverSocket);
    }
}

int Server::start()
{
    running = true;
    if (listen(serverSocket, 5) < 0)
    {
        Logger::generalLogMessage("manager", LogLevel::ERROR, "listenning error occurred");
        close(serverSocket);
        return -1;
    }
    Logger::generalLogMessage("manager", LogLevel::INFO, "server running on port " + port);

    while (running)
    {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket < 0)
        {
            Logger::generalLogMessage("manager", LogLevel::ERROR, "accepting error occurred");
            return -1;
        }
        Logger::generalLogMessage("manager", LogLevel::INFO, "connection succeed to client socket number: " + clientSocket);

        std::lock_guard<std::mutex> lock(socketMutex);
        sockets.push_back(clientSocket);

#ifndef BROADCAST_MODE
        Packet packet;
        int valread = recv(clientSocket, &packet, sizeof(Packet), 0);
        if (valread <= 0)
        {
            Logger::generalLogMessage("manager", LogLevel::ERROR, "Connection closed or error occurred");
            break;
        }
        Logger::packetLogMessage("manager", LogLevel::INFO, "recieve ID for init", packet);
        uint32_t clientID = packet.header.SrcID;
        clientIDMap[clientSocket] = clientID;
#endif

        // Openes a new thread for : handleClient- listening to messages from the process
        clientThreads.emplace_back(&Server::handleClient, this, clientSocket);
    }
    return 0;
}

void Server::stop()
{
    running = false;
    Logger::generalLogMessage("manager", LogLevel::INFO, "close manager socket number: " + serverSocket);
    close(serverSocket);

    for (int sock : sockets)
    {
        Logger::generalLogMessage("manager", LogLevel::INFO, "close client socket number: " + sock);
        close(sock);
    }
    for (auto &th : clientThreads)
    {
        if (th.joinable())
            th.join();
    }
}

void Server::handleClient(int clientSocket)
{
    Packet packet;
    while (true)
    {
        int valread = recv(clientSocket, &packet, sizeof(Packet), 0);
        if (valread <= 0)
        {
            Logger::generalLogMessage("manager", LogLevel::ERROR, "Connection closed or error occurred in client socket number: " + clientSocket);
            break;
        }
        Logger::packetLogMessage("manager", LogLevel::INFO, "recieved packet", packet);
        receiveData(&packet);
    }
    // If the process is no longer connected
    std::lock_guard<std::mutex> lock(socketMutex);
    auto it = std::find(sockets.begin(), sockets.end(), clientSocket);
    if (it != sockets.end())
        sockets.erase(it);
}

int Server::getClientSocketByID(uint32_t destID)
{
    for (const auto &client : clientIDMap)
    {
        if (client.second == destID)
            return client.first;
    }
    return -1; // אם לא נמצא
}

int Server::sendToClients(const Packet &packet)
{
#ifdef BROADCAST_MODE
    std::lock_guard<std::mutex> lock(socketMutex);
    for (int sock : sockets)
    {
        ssize_t bytesSent = send(sock, &packet, sizeof(Packet), 0);
        if (bytesSent < sizeof(Packet))
        {
            Logger::packetLogMessage("manager", LogLevel::ERROR, "sending failed", packet);
            return -1;
        }
        Logger::packetLogMessage("manager", LogLevel::INFO, "sending succeed", packet);
    }
#else
    int targetSocket = getClientSocketByID(packet.header.DestID);
    if (targetSocket != -1)
    {
        ssize_t bytesSent = send(targetSocket, &packet, sizeof(Packet), 0);
        if (bytesSent < sizeof(Packet))
        {
            Logger::packetLogMessage("manager", LogLevel::ERROR, "sending failed", packet);
            return -1;
        }
        Logger::packetLogMessage("manager", LogLevel::INFO, "sending succeed", packet);
    }
#endif
    return 0;
}
