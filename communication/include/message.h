#pragma once
#include <vector>
#include <cstring>
#include <cstdint>
#include <ctime>
#include <cstdlib>
#include <unordered_map>
#include <iostream>
#include <chrono>
#include "packet.h"

enum MessageType {
    INITIAL_CONNECTION,           // Represents initial connection of a component to the server
    ERROR_MESSAGE,                // Error message (highest priority)
    ACK,                          // Acknowledgment message
    OVERLOAD_MESSAGE,             // Indicates network overload
    REMOTE_TRANSMISSION_REQUEST,  // Request for remote data (RTR)
    DATA_MESSAGE                  // Regular data message (lowest priority)
};



class Message {
private:
    std::vector<Packet> packets;   
    uint32_t tps;                 
    uint32_t messageID;           

    uint32_t generateMessageID(MessageType messageType, uint16_t srcID);

public:
    // Default constructor
    Message() = default;

    // Constructor for sending message
    Message(uint32_t srcID, uint32_t destID, void* data, size_t size, MessageType messageType);

    // Constructor for receiving message
    Message(uint32_t tps);

    // Add a packet to the received message
    bool addPacket(const Packet &p);

    // Check if the message is complete
    bool isComplete() const;

    // Get the complete data of the message
    void* completeData() const;

    // Get the packets of the message
    std::vector<Packet>& getPackets();

    // Extract the MessageType from the message ID
    static MessageType getMessageTypeFromID(uint32_t messageID);

    // Returns a string representation of the MessageType enum value
    static std::string getMessageTypeString(MessageType type);

    // Get the message ID
    uint32_t getMessageID() const
    {
        return messageID;
    }
};