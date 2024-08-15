#include "message.h"

// Packet definition

// Constructor to initialize Packet for sending
Packet::Packet(uint32_t psn, uint32_t tps, uint32_t srcID, uint32_t destID, uint8_t *data, int dlc, bool isBroadcast, bool RTR = false, bool passive = false)
{
    header.PSN = psn;
    header.TPS = tps;
    header.SrcID = srcID;
    header.DestID = destID;
    header.DLC = dlc;
    std::memcpy(this->data, data, dlc);
    header.CRC = calculateCRC(data, dlc);
    header.timestamp = std::time(nullptr);
    header.RTR = RTR;
    header.passive = passive;
    header.isBroadcast = isBroadcast;
}

// Constructor to initialize receiving Packet ID for init
Packet::Packet(uint32_t id)
{
    std::memset(&header, 0, sizeof(header)); // Initialize all fields to zero
    header.SrcID = id;
    header.timestamp = std::time(nullptr);
}

// Implementation according to the CAN BUS
uint16_t Packet::calculateCRC(const uint8_t *data, size_t length)
{
    uint16_t crc = 0xFFFF;
    // Calculate CRC for the given data and length
    return crc;
}

// Message definition

// Constructor for sending message
Message::Message(uint32_t srcID, uint32_t destID = 0xFFFF, void *data, int dlc, bool isBroadcast)
{
    size_t size = dlc;
    uint32_t tps = (size + 7) / 8; // Calculate the number of packets needed
    for (uint32_t i = 0; i < tps; ++i) {
        uint8_t packetData[8];
        size_t copySize = std::min(size - i * 8, (size_t)8); // Determine how much data to copy for each packet
        std::memcpy(packetData, (uint8_t *)data + i * 8, copySize);
        packets.emplace_back(i, tps, srcID, destID, packetData, copySize, isBroadcast);
    }
}

// Constructor for receiving message
Message::Message(uint32_t tps)
{
    this->tps = tps;
}

// Add a packet to the received message
bool Message::addPacket(const Packet &p)
{
    // Implementation according to the CAN BUS
    
    // For further testing
    // if (p.header.PSN >= packets.size()) {
    //     return false;
    // }

    packets.push_back(p);
    return true;
}

// Check if the message is complete
bool Message::isComplete() const
{
    return packets.size() == tps;
}

// Get the complete data of the message
void *Message::completeData() const
{
    size_t totalSize = (tps - 1) * 8 + packets.back().header.DLC;
    void *data = malloc(totalSize);
    for (const auto &packet : packets) {
        std::memcpy((uint8_t *)data + packet.header.PSN * 8, packet.data, packet.header.DLC);
    }
    return data;
}

// Get the packets of the message
std::vector<Packet> &Message::getPackets()
{
    return packets;
}
