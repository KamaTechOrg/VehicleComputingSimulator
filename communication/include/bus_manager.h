#pragma once
#include <mutex>
#include <utility>
#include "server_connection.h"
#include <iostream>
#include "Imanager.h"

class BusManager:IManager{
private:
    ServerConnection server;
    static BusManager* instance;
    static std::mutex managerMutex;
    std::mutex processesIdMutex;

    //callback 
    std::function<ErrorCode(Packet&)> recievedMessageCallback;
    std::function<ErrorCode(const uint32_t ,const uint32_t)> processJoinCallback;

public:
    BusManager(uint32_t port, std::function<ErrorCode(Packet&)> recievedMessageCallback , std::function<ErrorCode(const uint32_t ,const uint32_t)> processJoinCallback);

    // Sends to the server to listen for requests
    ErrorCode startConnection() override;

    // Receives the packet that arrived and checks it before sending it out
    ErrorCode receiveMessage(Packet &packet) override;

    // Receives the processId that arrived
    ErrorCode receiveNewProcessID(const uint32_t processID ,const uint32_t port) override;

     // Sending according to broadcast variable
    ErrorCode sendMessage(const Packet &packet) override;

    // Implementation according to the conflict management of the CAN bus protocol
    Packet checkCollision(Packet &currentPacket);

    // Implement a priority check according to the CAN bus
    Packet packetPriority(Packet &a, Packet &b);

    // Static method to handle SIGINT signal
    static void signalHandler(int signum);

    //close the manager
    ErrorCode closeConnection() override;

    ~BusManager();
};