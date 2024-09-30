#pragma once
#include <mutex>
#include <utility>
#include "server_connection.h"
#include <iostream>
#include "Imanager.h"

class BusManager:IManager{
private:
    ServerConnection server;
    std::mutex processesIdMutex;

    //callback 
    std::function<ErrorCode(Packet&)> recievedMessageCallback;
    std::function<ErrorCode(const uint32_t ,const uint32_t, bool)> processJoinCallback;

public:
    BusManager(uint32_t port, std::function<ErrorCode(Packet&)> recievedMessageCallback , std::function<ErrorCode(const uint32_t ,const uint32_t, bool)> processJoinCallback);

    // Sends to the server to listen for requests
    ErrorCode startConnection() override;

    // Receives the packet that arrived and checks it before sending it out
    ErrorCode receiveMessage(Packet &packet) override;

    // Receives the processId that arrived
    ErrorCode updateProcessID(const uint32_t processID ,const uint32_t port, bool isConnected) override;

     // Sending according to broadcast variable
    ErrorCode sendMessage(const Packet &packet) override;

    // Implementation according to the conflict management of the CAN bus protocol
    Packet checkCollision(Packet &currentPacket);

    // Implement a priority check according to the CAN bus
    Packet packetPriority(Packet &a, Packet &b);

    //close the manager
    ErrorCode closeConnection() override;
    
    ~BusManager();
};