#include "../include/packet.h"
// Constructor to initialize Packet for sending
Packet::Packet(uint32_t id, uint32_t psn, uint32_t tps, uint32_t srcID, uint32_t destID, uint8_t *data, int dlc, bool isBroadcast, bool RTR, bool passive)
{
    header.ID = id;
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