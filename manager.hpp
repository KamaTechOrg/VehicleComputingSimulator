#pragma once
#include <mutex>
#include <utility>
#include "server.hpp"
#include <iostream>

class Manager
{
private:
    Server server;
    std::pair<std::time_t, Packet> lastPacket;
    std::mutex lock;

public:
    // constructor
    Manager();

    // Sends to the server to listen for requests
    void start();

    // Receives the packet that arrived and checks it before sending it out
    void receiveData(void *data);

    // Implementation according to the conflict management of the CAN bus protocol
    static Packet checkCollision(Packet &currentPacket);

    // Implement a priority check according to the CAN bus
    static Packet packetPriority(Packet &a, Packet &b);
};
