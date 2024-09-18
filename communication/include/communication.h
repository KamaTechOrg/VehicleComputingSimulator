#pragma once
#include <unordered_map>
#include <csignal>
#include "client_connection.h"
#include "../sockets/Isocket.h"
#include "error_code.h"
#include "sync_communication.h"
#include <future>

#ifdef ESP32
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

class Communication
{
private:
    uint32_t processID;
    ClientConnection client;
    std::unordered_map<std::string, Message> receivedMessages;
    void (*passData)(uint32_t, void *);  
    // A static variable that holds an instance of the class
    static Communication* instance;

#ifdef ESP32
    TaskHandle_t receiveTaskHandle; // Handle for the FreeRTOS task
#endif

    // Accepts the packet from the client and checks..
    void receivePacket(const Packet &p);
    
    // Checks if the packet is intended for him
    bool checkDestId(const Packet &p);
    
    // Checks if the Packet is correct
    bool validCRC(Packet &p);
    
    // Receives the packet and adds it to the message
    void handlePacket(const Packet &p);
    
    // Implement error handling according to CAN bus
    void handleError();
    
    // Implement arrival confirmation according to the CAN bus
    Packet hadArrived();
    
    // Adding the packet to the complete message
    void addPacketToMessage(const Packet &p);

    // Static method to handle signal (replace with FreeRTOS task deletion)
    static void signalHandler(int signum);

    void setId(uint32_t newId);

    void setPassDataCallback(void (*callback)(uint32_t, void *));

    void setSignaleHandler();

public:
    // Constructor
    Communication(uint32_t id, void (*passDataCallback)(uint32_t, void *));
    
    // Sends the client to connect to server
    ErrorCode startConnection();
    
    // Sends a message to manager
    ErrorCode sendMessage(void *data, size_t dataSize, uint32_t destID, uint32_t srcID, bool isBroadcast);
    
    // Sends a message to manager - Async
    void sendMessageAsync(void *data, size_t dataSize, uint32_t destID, uint32_t srcID, std::function<void(ErrorCode)> passSend, bool isBroadcast);

    // Destructor
    ~Communication();
};