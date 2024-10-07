#include "../include/bus_manager.h"

// Initialize static instance to nullptr
BusManager *BusManager::instance = nullptr;
std::mutex BusManager::managerMutex;

// Private constructor: initializes the server and starts the collision timer
BusManager::BusManager(std::vector<uint32_t> idShouldConnect, uint32_t limit)
    : server(8080,
             std::bind(&BusManager::receiveData, this, std::placeholders::_1)),
      lastPacket(nullptr), stopFlag(false)
{
    startCollisionTimer();       // Start collision management timer
}

// Singleton getter: ensures only one instance of Manager
BusManager *BusManager::getInstance(std::vector<uint32_t> idShouldConnect,
                              uint32_t limit)
{
    if (instance == nullptr) {
        std::lock_guard<std::mutex> lock(managerMutex);
        if (instance == nullptr) {
            instance = new BusManager(idShouldConnect, limit);
        }
    }
    return instance;
}

// Starts the server connection and listens for incoming requests
ErrorCode BusManager::startConnection()
{
    return server.startConnection();
}

// Receives a packet, checks for collisions, and sends it if valid
void BusManager::receiveData(Packet &p)
{
    checkCollision(p); // Handle packet collisions
}

// Sends a packet either as broadcast or to a specific destination
ErrorCode BusManager::sendToClients(const Packet &packet)
{
    if (packet.getIsBroadcast()) {
        return server.sendBroadcast(packet); // Broadcast message
    }
    return server.sendDestination(packet); // Send to specific client
}

// Manages collisions based on the CAN BUS protocol
void BusManager::checkCollision(Packet &currentPacket)
{
    std::lock_guard<std::mutex> lock(lastPacketMutex);
    if (lastPacket == nullptr) {
        // No previous packet, store the current one
        lastPacket = new Packet(currentPacket);
        if (lastPacket == nullptr) {
        }
    }
    else {
        if (lastPacket->getTimestamp() == currentPacket.getTimestamp()) {
            RealSocket::log.logMessage(
                logger::LogLevel::INFO, "Handled collision between packet from SRC " 
                                        + std::to_string(lastPacket->getSrcId())
                                        + " to DST " + std::to_string(lastPacket->getDestId())
                                        + ", (packet number: " + std::to_string(lastPacket->getPSN())
                                        + " of messageId " + std::to_string(lastPacket->getId())
                                        + ") and packet from SRC " + std::to_string(currentPacket.getSrcId())
                                        + " to DST " + std::to_string(currentPacket.getDestId())
                                        + ", (packet number: " + std::to_string(currentPacket.getPSN())
                                        + " of messageId " + std::to_string(currentPacket.getId())
                                        + ")."
            );

            // Same timestamp indicates potential collision, check priority
            Packet* prioritizedPacket = packetPriority(*lastPacket, currentPacket);

            // Log the result once and determine the lost packet
            Packet* lostPacket = (prioritizedPacket == &currentPacket) ? lastPacket : &currentPacket;

            // Update the last packet if current has priority
            if (prioritizedPacket == &currentPacket) {
                delete lastPacket; // Replace last packet
                lastPacket = new Packet(currentPacket);
            }

            RealSocket::log.logMessage(
                logger::LogLevel::INFO, "Packet from SRC "
                                        + std::to_string(prioritizedPacket->getSrcId()) 
                                        + " to DST " + std::to_string(prioritizedPacket->getDestId())
                                        + ", (packet number: " + std::to_string(prioritizedPacket->getPSN())
                                        + " of messageId " + std::to_string(prioritizedPacket->getId())
                                        + " ) won the collision. Packet from SRC " + std::to_string(lostPacket->getSrcId())
                                        + " to DST " + std::to_string(lostPacket->getDestId())
                                        + ", (packet number: " + std::to_string(lostPacket->getPSN())
                                        + " of messageId " + std::to_string(lostPacket->getId()) + ") was lost."
            );
        }
    }
}

// Determines which packet has higher priority (CAN BUS logic)
Packet *BusManager::packetPriority(Packet &a, Packet &b)
{
    if (a.getIsPassive() && !b.getIsPassive()) {
        return &b; // Non-passive packet takes priority
    }
    else if (!a.getIsPassive() && b.getIsPassive()) {
        return &a; // Non-passive packet takes priority
    }
    else {
        return (a > b) ? &a
                       : &b; // If both are the same type, compare based on ID
    }
}

// Starts the collision timer to periodically check for packet collisions
void BusManager::startCollisionTimer()
{
    stopFlag = false;
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Starting collision timer thread."); // לוג לפני התחלת ה-thread

    collisionTimerThread = std::thread([this]() {
        while (!stopFlag) {
            GlobalClock::waitForNextTick(); // מחכה לשעון

            if (!stopFlag)
                checkLastPacket(); // Check and send last packet if necessary
        }
        
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Collision timer thread stopping."); // לוג כאשר ה-thread מפסיק
    });
}


// Checks the last packet and sends it if it hasn't been sent yet
void BusManager::checkLastPacket()
{
    std::lock_guard<std::mutex> lock(lastPacketMutex);
    if (lastPacket != nullptr) {
        ErrorCode res = sendToClients(*lastPacket);
        delete lastPacket; // Clear last packet after sending
        lastPacket = nullptr;
    }
}

//shut down the server
void BusManager::stopConnection()
{
    if (instance) {
        instance->server.stopServer(); // Stop server on interrupt
    }
}

// Destructor: ensures proper cleanup of threads and resources
BusManager::~BusManager()
{
    stopFlag = true; // Stop collision timer thread
    if (collisionTimerThread.joinable()) {
        collisionTimerThread.join();
    }
    delete lastPacket; // Clean up lastPacket
    instance = nullptr;
}