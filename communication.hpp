#pragma once
#include <unordered_map>
#include "client.hpp"

class Communication
{
private:
    Client client;
    std::unordered_map<std::string, Message> receivedMessages;
    void (*passData)(void *);
    uint32_t id;

public:
    // Constructor
    Communication(uint32_t id, void (*passDataCallback)(void *));
    // Sends the client to connect to server
    void start();
    
    // Sends a message from destID to srcID
    void sendMessage(void *data, size_t dataSize, uint32_t destID, uint32_t srcID);
    
    // Accepts the packet from the client and checks..
    void receivePacket(Packet &p);
    
    // Checks if the packet is intended for him
    bool checkDestId(Packet &p);
    
    // Checks if the Packet is currect
    bool validCRC(Packet &p);
    
    // Receives the packet and adds it to the message
    void handlePacket(Packet &p);
    
    // Implement error handling according to CAN bus
    void handleError();
    
    // Implement arrival confirmation according to the CAN bus
    Packet hadArrived();
    
    // Adding the packet to the complete message
    void addPacketToMessage(Packet &p);
    
};