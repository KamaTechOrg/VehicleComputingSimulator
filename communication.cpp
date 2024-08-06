#include "communication.hpp"

Communication::Communication(uint32_t id, void (*passDataCallback)(void *)) : client(id, std::bind(&Communication::receivePacket, this, std::placeholders::_1)), passData(passDataCallback), id(id) {}

void Communication::start()
{
    client.connectToServer();
}

void Communication::sendMessage(void *data, size_t dataSize, uint32_t destID, uint32_t srcID)
{
    // Creating a message and dividing it into packets
    Message msg(srcID, destID, data, dataSize);
    // Sending them
    for (auto &packet : msg.getPackets())
        client.sendPacket(packet);
}

void Communication::receivePacket(Packet &p)
{
    if (checkDestId(p))
    {
        if (validCRC(p))
            handlePacket(p);
        else
            handleError();
    }
}

bool Communication::checkDestId(Packet &p)
{
    return p.header.DestID == this->id;
}

bool Communication::validCRC(Packet &p)
{
    return p.header.CRC == p.calculateCRC(p.data, p.header.DLC);
}

void Communication::handlePacket(Packet &p)
{
    // Send acknowledgment according to CAN bus
    // client.sendPacket(hadArrived());
    addPacketToMessage(p);
}

void Communication::handleError()
{
    // Handle error cases according to CAN bus
}

Packet Communication::hadArrived()
{
    Packet ack;
    // Construct and return acknowledgment packet
    return ack;
}

void Communication::addPacketToMessage(Packet &p)
{
    // messageId may have to change according to the CAN bus
    std::string messageId = std::to_string(p.header.SrcID) + "-" + std::to_string(p.header.DestID);
    // If the message already exists, we will add the packet
    if (receivedMessages.find(messageId) != receivedMessages.end())
    {
        receivedMessages[messageId].addPacket(p);
    }
    else
    {
        // If the message does not exist, we will create a new message
        Message msg(p.header.TPS);
        msg.addPacket(p);
        receivedMessages[messageId] = msg;
    }
    // If the message is complete, we pass the data to the passData function
    if (receivedMessages[messageId].isComplete())
    {
        void *completeData = receivedMessages[messageId].completeData();
        passData(completeData);
        receivedMessages.erase(messageId); // Removing the message once completed
    }
}
