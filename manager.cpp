#include "manager.hpp"

Manager::Manager() : server(8080, std::bind(&Manager::receiveData, this, std::placeholders::_1)) {}

void Manager::start()
{
    server.start();
}

void Manager::receiveData(void *data)
{
    Packet *p = static_cast<Packet *>(data);
    server.sendToClients(*p);
    // Checking the case of collision and priority in functions : checkCollision,packetPriority
    // Packet* resolvedPacket = checkCollision(*p);
    // if (resolvedPacket)
    //     server.sendToClients(*resolvedPacket);
}

Packet Manager::checkCollision(Packet &currentPacket)
{
    return currentPacket;
}

Packet Manager::packetPriority(Packet &a, Packet &b)
{
    return (a.header.SrcID < b.header.SrcID) ? a : b;
}
