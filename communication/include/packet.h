#ifndef __PACKET_H__
#define __PACKET_H__

#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>

#define SIZE_PACKET 8

class Packet {
public:
  // Default constructor for Packet.
  Packet() = default;

  // Constructor for sending message
  Packet(uint32_t id, uint32_t PSN, uint32_t TPS, uint32_t srcId,
         uint32_t destId, int DLC, bool RTR, bool isBroadcast,
         const uint8_t *payload);

  // Constructor for receiving message
  Packet(uint32_t id);

  // Destructor
  ~Packet() = default;

  // Overloaded operator>
  bool operator>(const Packet &other) const;

  // Getters for accessing Header fields
  uint32_t getId() const { return header.id; }
  uint32_t getPSN() const { return header.PSN; }
  uint32_t getTPS() const { return header.TPS; }
  uint32_t getSrcId() const { return header.srcId; }
  uint32_t getDestId() const { return header.destId; }
  int getTimestamp() const { return header.timestamp; }
  int getDLC() const { return header.DLC; }
  uint16_t getCRC() const { return header.CRC; }
  bool isRTR() const { return header.RTR; }
  bool getIsPassive() const { return header.passive; }
  bool getIsBroadcast() const { return header.isBroadcast; }

  const uint8_t *getPayload() const { return payload; }

  // Setters for modifying Header fields
  void setIsPassive(bool p) { header.passive = p; }
  void setTimestamp(int t) { header.timestamp = t; }

  // CRC calculation
  uint16_t calculateCRC() const;
  bool validateCRC() const;

  // Static function to convert raw data to hexadecimal
  static std::string pointerToHex(const void* data, size_t size);
private:
  // Header structure within Packet
  struct Header {
    uint32_t id;      // Unique identifier for the message (4 bytes)
    uint32_t PSN;     // Packet Sequence Number (4 bytes)
    uint32_t TPS;     // Total Packet Sum (4 bytes)
    uint32_t srcId;   // Source node ID (4 bytes)
    uint32_t destId;  // Destination node ID (4 bytes)
    int timestamp;    // Timestamp marking the packet's send time (4 bytes)
    int DLC;          // Data Length Code (0-8 bytes) (4 bytes)
    uint16_t CRC;     // Cyclic Redundancy Check (2 bytes)
    bool RTR;         // Remote Transmission Request flag (1 byte)
    bool passive;     // Passive state flag (1 byte)
    bool isBroadcast; // Broadcast flag (1 byte)
  } header;

  uint8_t payload[SIZE_PACKET]; // Data payload (fixed size, up to SIZE_PACKET bytes)
};

#endif // __PACKET_H__