#pragma once
#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <functional>
#include <iostream>
#include "message.hpp"
#include "logger.hpp"
#include <string>

#define PORT 8080
#define IP "127.0.0.1"

class Client
{
private:
    uint32_t id;
    int clientSocket;
    sockaddr_in servAddress;
    bool connected;
    std::function<void(Packet &)> passPacketCom;

public:
    // Constructor
    Client(uint32_t id, std::function<void(Packet &)> callback);

    // Requesting a connection to the server
    int connectToServer();

    // Sends the packet to the manager-sync
    int sendPacket(Packet &packet);

    // Sends the packet to the manager-async
    int sendPacketAsync(Packet &packet);

    // Waits for a message and forwards it to Communication
    void receivePacket();

    // Closes the connection
    void closeConnection();
};