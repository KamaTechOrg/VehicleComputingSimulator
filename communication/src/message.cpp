#include "../include/message.h"

// Constructor for sending message
Message::Message(uint32_t srcID, uint32_t destID, void* data, size_t size, MessageType messageType) 
{
    messageID = generateMessageID(messageType, srcID);
    tps = (size + SIZE_PACKET-1) / SIZE_PACKET; // Calculate the number of packets needed
    bool isBroadcast = (messageType == MessageType::ERROR_MESSAGE);
    bool isRTR = (messageType == MessageType::REMOTE_TRANSMISSION_REQUEST);
    for (uint32_t i = 0; i < tps; ++i) {
        uint8_t packetData[SIZE_PACKET];
        size_t copySize = std::min(size - i * SIZE_PACKET, (size_t)SIZE_PACKET); // Determine how much data to copy for each packet
        std::memcpy(packetData, (uint8_t*)data + i * SIZE_PACKET, copySize);
        packets.emplace_back(messageID, i, tps, srcID, destID, copySize, isRTR, isBroadcast, packetData);
    }
}

uint32_t Message::generateMessageID(MessageType messageType, uint16_t srcID) {
    uint32_t messageTypeID = static_cast<uint32_t>(messageType);
    uint32_t srcID32 = static_cast<uint32_t>(srcID);
    
    auto now = std::chrono::system_clock::now().time_since_epoch();
    uint32_t timestamp = static_cast<uint32_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(now).count() & 0xFF
    );

    return (messageTypeID << 24) | 
           (srcID32 << 8) | 
           timestamp;
}

// Constructor for receiving message
Message::Message(uint32_t tps)
{
    this->tps = tps;
}

// Add a packet to the received message
bool Message::addPacket(const Packet &p)
{
    if (!packets.empty()) {
        if (p.getPSN() <= packets.back().getPSN()) {
            return false;
        }
    }
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
    size_t totalSize = (tps - 1) * SIZE_PACKET + packets.back().getDLC();
    void *data = malloc(totalSize);
    for (const auto &packet : packets) {
        std::memcpy((uint8_t *)data + packet.getPSN() * SIZE_PACKET, packet.getPayload(), packet.getDLC());
    }
    return data;
}

// Get the packets of the message
std::vector<Packet> &Message::getPackets()
{
    return packets;
}

// Extract the MessageType from the message ID
MessageType Message::getMessageTypeFromID(uint32_t messageID)
{
    uint32_t messageTypeID = (messageID >> 24) & 0xFF;  // Extract the message type
    return static_cast<MessageType>(messageTypeID);      // Cast to MessageType
}

// Returns a string representation of the MessageType enum value
std::string Message::getMessageTypeString(MessageType type)
{
    switch (type) {
        case MessageType::INITIAL_CONNECTION: return "INITIAL_CONNECTION";
        case MessageType::ERROR_MESSAGE: return "ERRORE";
        case MessageType::ACK: return "ACK";
        case MessageType::OVERLOAD_MESSAGE: return "OVERLOAD";
        case MessageType::REMOTE_TRANSMISSION_REQUEST: return "RTRT";
        case MessageType::DATA_MESSAGE: return "DATA";
        default: return "UNKNOWN";
    }
}