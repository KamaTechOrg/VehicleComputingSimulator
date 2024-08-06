#include "client.hpp"

Client::Client(uint32_t id, std::function<void(Packet &)> callback) : id(id), passPacketCom(callback), connected(false)
{
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        Logger::generalLogMessage(std::to_string(id), LogLevel::ERROR, "failed to create a client socket");
        return;
    }
    Logger::generalLogMessage(std::to_string(id), LogLevel::INFO, "create a client socket");
    servAddress.sin_family = AF_INET;
    servAddress.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &servAddress.sin_addr);
}

int Client::connectToServer()
{
    if (connect(clientSocket, (struct sockaddr *)&servAddress, sizeof(servAddress)) < 0)
    {
        Logger::generalLogMessage(std::to_string(id), LogLevel::ERROR, "error to connect to server");
        return -1;
    }

    Logger::generalLogMessage(std::to_string(id), LogLevel::INFO, "connection succeed to server socket");
    connected = true;
#ifndef BROADCAST_MODE
    Packet packet(id);
    ssize_t bytesSent = send(clientSocket, &packet, sizeof(Packet), 0);
    if (bytesSent < sizeof(Packet))
    {
        Logger::packetLogMessage(std::to_string(id), LogLevel::ERROR, "sending id for init failed", packet);
        return -1;
    }
    Logger::packetLogMessage(std::to_string(id), LogLevel::INFO, "sending id for init succeed", packet);

#endif
    // std::cout << "client connect to server" << std::endl;
    std::thread(&Client::receivePacket, this).detach();
    return 0;
}

int Client::sendPacket(Packet &packet)
{
    if (!connected)
        return -1;

    ssize_t bytesSent = send(clientSocket, &packet, sizeof(Packet), 0);
    if (bytesSent < sizeof(Packet))
    {
        Logger::packetLogMessage(std::to_string(id), LogLevel::ERROR, "sending failed", packet);
        return -1;
    }
    Logger::packetLogMessage(std::to_string(id), LogLevel::INFO, "sending succeed", packet);
    return 0;
}

int Client::sendPacketAsync(Packet &packet)
{
    // std::thread(&Client::sendPacket, this, packet).detach();
    Logger::packetLogMessage(std::to_string(id), LogLevel::INFO, "sending succeed", packet);
    return 0;
}

void Client::receivePacket()
{
    Packet packet;
    while (true)
    {
        int valread = recv(clientSocket, &packet, sizeof(Packet), 0);
        if (valread <= 0)
        {
            Logger::generalLogMessage(std::to_string(id), LogLevel::ERROR, "Connection closed or error occurred in client socket number: " + clientSocket);
            break;
        }
        Logger::packetLogMessage(std::to_string(id), LogLevel::INFO, "recieved packet", packet);
        passPacketCom(packet);
    }
}

void Client::closeConnection()
{
    close(clientSocket);
    Logger::generalLogMessage(std::to_string(id), LogLevel::INFO, "close client socket number: " + clientSocket);
    connected = false;
}
