#pragma once
#include <unordered_map>
#include <csignal>
#include "client_connection.h"
#include "error_code.h"

#ifdef ESP32
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

class Communication
{
private:
    ClientConnection client;
    std::unordered_map<std::string, Message> receivedMessages;
    void (*passData)(void *); 
    uint32_t id;
    static Communication* instance;

#ifdef ESP32
    TaskHandle_t receiveTaskHandle; // Handle for the FreeRTOS task
#endif

    // Accepts the packet from the client and checks..
    void receivePacket(Packet &p);
    
    // Checks if the packet is intended for him
    bool checkDestId(Packet &p);
    
    // Checks if the Packet is correct
    bool validCRC(Packet &p);
    
    // Receives the packet and adds it to the message
    void handlePacket(Packet &p);
    
    // Implement error handling according to CAN bus
    void handleError();
    
    // Implement arrival confirmation according to the CAN bus
    Packet hadArrived();
    
    // Adding the packet to the complete message
    void addPacketToMessage(Packet &p);

    // Static method to handle signal (replace with FreeRTOS task deletion)
    static void signalHandler(int signum);

    void setId(uint32_t newId);

    void setPassDataCallback(void (*callback)(void *));

public:
    // Constructor
    Communication(uint32_t id, void (*passDataCallback)(void *));
    
    // Sends the client to connect to server
    ErrorCode startConnection();
    
    // Sends a message to manager
    ErrorCode sendMessage(void *data, size_t dataSize, uint32_t destID, uint32_t srcID, bool isBroadcast);
    
    // Sends a message to manager - Async
    void sendMessageAsync(void *data, size_t dataSize, uint32_t destID, uint32_t srcID, std::function<void(ErrorCode)> passSend, bool isBroadcast);

    // Destructor
    ~Communication();
};