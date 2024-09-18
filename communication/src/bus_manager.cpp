#include "../include/bus_manager.h"

//Private constructor
BusManager::BusManager(uint16_t port, std::function<ErrorCode(Packet&)> recievedMessageCallback , std::function<ErrorCode(const uint32_t ,const uint32_t,bool)> processJoinCallback)
 : server(port, std::bind(&BusManager::receiveMessage, this, std::placeholders::_1), std::bind(&BusManager::updateProcessID, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
 ,recievedMessageCallback(recievedMessageCallback) ,processJoinCallback(processJoinCallback){}

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

ErrorCode BusManager::updateProcessID(const uint32_t processID, const uint16_t port, bool isConnected)
{
    return processJoinCallback(processID ,port,isConnected);
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

ErrorCode BusManager::closeConnection()
{
    return server.stopServer();
}

BusManager::~BusManager()
{
    server.stopServer();
}