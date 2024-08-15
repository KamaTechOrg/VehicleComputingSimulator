#include "manager.h"

// constructor
Manager::Manager() : server(8080, std::bind(&Manager::receiveData, this, std::placeholders::_1)) {}

// Sends to the server to listen for requests
int Manager::startConnection()
{
    return server.startConnection();
}

// Receives the packet that arrived and checks it before sending it out
void Manager::receiveData(void *data)
{
    Packet *p = static_cast<Packet *>(data);
    int res = sendToClients(*p);
    // Checking the case of collision and priority in functions : checkCollision,packetPriority
    // Packet* resolvedPacket = checkCollision(*p);
    // if (resolvedPacket)
    //     server.sendToClients(*resolvedPacket);
}

// Sending according to broadcast variable
int Manager::sendToClients(const Packet &packet)
{
    if(packet.header.isBroadcast)
        return server.sendBroadcast(packet);
    return server.sendDestination(packet);
}

// Implement according to the conflict management of the CAN bus protocol
Packet Manager::checkCollision(Packet &currentPacket)
{
    return currentPacket;
}

// Implement a priority check according to the CAN bus
Packet Manager::packetPriority(Packet &a, Packet &b)
{
    return (a.header.SrcID < b.header.SrcID) ? a : b;
}

Manager::~Manager(){}