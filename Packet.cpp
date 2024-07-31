#include <cstdint>
#include <cstring>
#include "communication.h"

class Packet {
public:
    int getPsn() const { return PSN; }
    int getTotalPacketSum() const { return totalPacketSum; }
    uint8_t* getData() { return data; }
    void setPsn(int psn) { this->PSN = psn; }
    void setTotalPacketSum(int totalPacketSum) { this->totalPacketSum = totalPacketSum; }
    void setData(const void* inputData, size_t size) { std::memcpy(this->data, inputData, size); }
    
private:
    int PSN = 0;
    int totalPacketSum = 0;
    uint8_t data[communication::PACKET_SIZE] = {0};
};
