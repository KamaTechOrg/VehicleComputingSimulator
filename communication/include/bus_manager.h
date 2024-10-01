#pragma once
#include <mutex>
#include <thread>
#include <atomic>
#include <iostream>
#include "../include/server_connection.h"
#include "Imanager.h"
#include "packet.h"
#include "global_clock.h"

// Manager class responsible for handling CAN BUS-like communication and collision management
class BusManager:IManager {
private:
    ServerConnection server;
    std::mutex processesIdMutex;

    //callback 
    std::function<ErrorCode(Packet&)> recievedMessageCallback;
    std::function<ErrorCode(const uint32_t ,const uint16_t, bool)> processJoinCallback;
    
    //collision management
    Packet *lastPacket;             // Stores the last packet received
    std::mutex lastPacketMutex;     // Protects access to lastPacket
    std::atomic<bool> stopFlag; // Indicates if the collision timer should stop
    std::thread collisionTimerThread; // Thread for collision management

public:
    BusManager(uint16_t port, std::function<ErrorCode(Packet&)> recievedMessageCallback , std::function<ErrorCode(const uint32_t ,const uint32_t, bool)> processJoinCallback);

    // Sends to the server to listen for requests
    ErrorCode startConnection() override;

    // Receives the packet that arrived and checks it before sending it out
    ErrorCode receiveMessage(Packet &packet) override;

    // Receives the processId that arrived
    ErrorCode updateProcessID(const uint32_t processID ,const uint16_t port, bool isConnected) override;

     // Sending according to broadcast variable
    ErrorCode sendMessage(const Packet &packet) override;
   
    // Starts the timer to check for packet collisions
    void startCollisionTimer();

    // Checks if the current packet collides with the last one
    void checkCollision(Packet &currentPacket);

    // Determines packet priority in case of collision, based on CAN BUS protocol
    Packet *packetPriority(Packet &a, Packet &b);

    // Sends the last packet if necessary and clears it
    void checkLastPacket();

    //close the manager
    ErrorCode closeConnection() override;
    
    ~BusManager();
};