#pragma once
#include <mutex>
#include <thread>
#include <atomic>
#include "../include/server_connection.h"
#include <iostream>
#include "packet.h"
#include "global_clock.h"

// Manager class responsible for handling CAN BUS-like communication and collision management
class BusManager {
  public:
    // Returns the singleton instance of Manager
    static BusManager *getInstance(std::vector<uint32_t> idShouldConnect,
                                uint32_t limit);

    // Starts server connection
    ErrorCode startConnection();

    // Stops server connection
    static void stopConnection();

    // Receives a packet and checks for collisions before sending
    void receiveData(Packet &p);

    // Destructor for cleaning up
    ~BusManager();    
  private:
    ServerConnection server;                  // Handles communication with the server
    static BusManager *instance;       // Singleton instance
    static std::mutex managerMutex; // Mutex for singleton
    Packet *lastPacket;             // Stores the last packet received
    std::mutex lastPacketMutex;     // Protects access to lastPacket
    std::atomic<bool> stopFlag; // Indicates if the collision timer should stop
    std::thread collisionTimerThread; // Thread for collision management

    // Private constructor to ensure singleton pattern
    BusManager(std::vector<uint32_t> idShouldConnect, uint32_t limit);

    // Sends packet to clients based on whether it's broadcast or unicast
    ErrorCode sendToClients(const Packet &packet);

    // Starts the timer to check for packet collisions
    void startCollisionTimer();

    // Checks if the current packet collides with the last one
    void checkCollision(Packet &currentPacket);

    // Determines packet priority in case of collision, based on CAN BUS protocol
    Packet *packetPriority(Packet &a, Packet &b);

    // Sends the last packet if necessary and clears it
    void checkLastPacket();

};