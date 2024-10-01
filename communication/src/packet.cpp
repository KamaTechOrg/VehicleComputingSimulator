#include "../include/packet.h"
// Constructor to initialize Packet for sending
Packet::Packet(uint32_t id, uint32_t PSN, uint32_t TPS, uint32_t srcId,
               uint32_t destId, int DLC, bool RTR, bool isBroadcast,
               const uint8_t* payload)
{
    header.id = id;
    header.PSN = PSN;
    header.TPS = TPS;
    header.srcId = srcId;
    header.destId = destId;
    header.DLC = DLC;
    header.RTR = RTR;
    header.passive = false; // Default value
    header.timestamp = 0;   // Default value
    header.isBroadcast = isBroadcast;

    // Ensure DLC does not exceed payload size
    size_t copySize = std::min(static_cast<size_t>(DLC), sizeof(this->payload));

    // Copy payload into the payload array if DLC > 0
    if (DLC > 0 && payload)
    {
        std::memcpy(this->payload, payload, copySize);
    }
    else
    {
        std::memset(this->payload, 0, sizeof(this->payload)); // Zero out payload if no data
    }

    // Calculate and set CRC after setting other header fields
    header.CRC = calculateCRC();
}

// Constructor to initialize receiving Packet ID for init
Packet::Packet(uint32_t id)
{
    std::memset(&header, 0, sizeof(header)); // Initialize all fields to zero
    header.srcId = id;
}

// Overloaded operator>
bool Packet::operator>(const Packet &other) const
{
    return header.id < other.header.id;
}

// CRC calculation
uint16_t Packet::calculateCRC() const
{
    const uint16_t polynomial = 0x4599; // Polynomial for CRC-15 CAN
    uint16_t crc = 0x0000; // Initial CRC value for CAN Bus

    // Combine header fields for CRC calculation (without the CRC field itself)
    crc ^= header.id;
    crc = (crc << 1) ^ polynomial;

    crc ^= header.PSN;
    crc = (crc << 1) ^ polynomial;

    crc ^= header.TPS;
    crc = (crc << 1) ^ polynomial;

    crc ^= header.srcId;
    crc = (crc << 1) ^ polynomial;

    crc ^= header.destId;
    crc = (crc << 1) ^ polynomial;

    crc ^= header.DLC;
    crc = (crc << 1) ^ polynomial;

    crc ^= header.RTR;
    crc = (crc << 1) ^ polynomial;

    crc ^= header.isBroadcast;
    crc = (crc << 1) ^ polynomial;

    // Process payload bytes for CRC calculation
    for (int i = 0; i < header.DLC; ++i)
    {
        crc ^= payload[i] << 8;
        for (int j = 0; j < 8; ++j)
        {
            if (crc & 0x8000)
            {
                crc = (crc << 1) ^ polynomial;
            }
            else
            {
                crc <<= 1;
            }
        }
    }

    // Mask CRC to 15 bits as per the CAN protocol
    return crc & 0x7FFF;
}

// Validate CRC
bool Packet::validateCRC() const
{
    return calculateCRC() == header.CRC;
}

// A function to convert the data to hexa (logger)
std::string Packet::pointerToHex() const
{
    std::ostringstream oss; // Stream for constructing the hexadecimal output
    const uint8_t* data = getPayload(); // Get the payload data
    int dlc = getDLC(); // Get the data length code (DLC)

    for (int i = 0; i < dlc; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]);
    }

    return oss.str(); // Return the hex string representation of the payload
}