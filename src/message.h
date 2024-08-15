#pragma once
#include <vector>
#include <cstring>
#include <cstdint>
#include <ctime>
#include <cstdlib>
#include <unordered_map>
#include <iostream>

struct Packet
{
    // Packet header containing various metadata fields
    struct Header
    {
        uint32_t ID;      // Message ID
        uint32_t PSN;     // Packet Sequence Number
        uint32_t TPS;     // Total Packet Sum
        uint32_t SrcID;   // Source ID
        uint32_t DestID;  // Destination ID
        int DLC;          // Data Length Code (0-8 bits)
        uint16_t CRC;     // Cyclic Redundancy Check for error detection
        int timestamp;    // Timestamp field
        bool isBroadcast; // True for broadcast, false for unicas
        bool passive;
        bool RTR;
    } header;

    uint8_t data[8];

    // Default constructor for Packet.
    Packet() = default;

    // Constructor for sending message
    Packet(uint32_t psn, uint32_t tps, uint32_t srcID, uint32_t destID, uint8_t *data, int dlc, bool RTR, bool passive, bool isBroadcast);

    // Constructor for receiving message
    Packet(uint32_t id);

    // Calculate CRC for the given data and length
    uint16_t calculateCRC(const uint8_t *data, size_t length);
};

// Message definition
class Message
{
private:
    std::vector<Packet> packets;
    uint32_t tps;
                  
public:

    // Default
    Message() = default;

    // Constructor for sending message
    Message(uint32_t srcID, uint32_t destID, void *data, int dlc, bool isBroadcast);
    
    // Constructor for receiving message
    Message(uint32_t tps);

    // Add a packet to the received message
    bool addPacket(const Packet &p);

    // Check if the message is complete
    bool isComplete() const;

    // Get the complete data of the message
    void *completeData() const;

    // Get the packets of the message
    std::vector<Packet> &getPackets();
};