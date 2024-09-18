#include "../include/bus_manager.h"

BusManager* BusManager::instance = nullptr;
std::mutex BusManager::managerMutex;

//Private constructor
BusManager::BusManager(uint32_t port, std::function<ErrorCode(Packet&)> recievedMessageCallback , std::function<ErrorCode(const uint32_t ,const uint32_t)> processJoinCallback)
 : server(port, std::bind(&BusManager::receiveMessage, this, std::placeholders::_1), std::bind(&BusManager::receiveNewProcessID, this, std::placeholders::_1, std::placeholders::_2))
 ,recievedMessageCallback(recievedMessageCallback) ,processJoinCallback(processJoinCallback) //,syncCommunication(idShouldConnect, limit)
{
    // Setup the signal handler for SIGINT
    signal(SIGINT, BusManager::signalHandler);
}

// Sends to the server to listen for requests
ErrorCode BusManager::startConnection()
{
    ErrorCode isConnected = server.startConnection();
    //syncCommunication.notifyProcess()
    return isConnected;
}

// Receives the packet that arrived and checks it before sending it out
ErrorCode BusManager::receiveMessage(Packet &packet)
{
    ErrorCode res = sendMessage(packet);
    if (res == ErrorCode::INVALID_CLIENT_ID)
        recievedMessageCallback(packet);
    // Checking the case of collision and priority in functions : checkCollision,packetPriority
    // Packet* resolvedPacket = checkCollision(*p);
    // if (resolvedPacket)
    //     server.sendToClients(*resolvedPacket);
    return res;

}

ErrorCode BusManager::receiveNewProcessID(const uint32_t processID, const uint32_t port)
{
    return processJoinCallback(processID ,port);
}

// Sending according to broadcast variable
ErrorCode BusManager::sendMessage(const Packet &packet)
{
    if(packet.header.isBroadcast)
        return server.sendBroadcast(packet);
    return server.sendDestination(packet);
}

// Implement according to the conflict management of the CAN bus protocol
Packet BusManager::checkCollision(Packet &currentPacket)
{
    return currentPacket;
}

// Implement a priority check according to the CAN bus
Packet BusManager::packetPriority(Packet &a, Packet &b)
{
    return (a.header.SrcID < b.header.SrcID) ? a : b;
}

// Static method to handle SIGINT signal
void BusManager::signalHandler(int signum)
{
    if (instance) {
        instance->server.stopServer();  // Call the stopServer method
    }
    exit(signum);
}

ErrorCode BusManager::closeConnection()
{
    return ErrorCode();
}

BusManager::~BusManager()
{
    instance = nullptr;
}
