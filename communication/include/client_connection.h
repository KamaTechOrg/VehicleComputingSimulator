#pragma once

#ifdef ESP32
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <WiFi.h>
#include <functional>
#include <atomic>
#include <string>
#include "message.h"
#include "../sockets/Isocket.h"
#include "error_code.h"
#include "../sockets/ESP32_socket.h"
#else
#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <functional>
#include <iostream>
#include <atomic>
#include <string>
#include <nlohmann/json.hpp>
#include "message.h"
#include "../sockets/Isocket.h"
#include "../sockets/real_socket.h"
#include "error_code.h"
#endif

class ClientConnection
{
private:
#ifdef ESP32
    TaskHandle_t receiveTaskHandle;
#else
    std::thread receiveThread;
#endif
    const char* serverIP;
    uint16_t serverPort;
    uint32_t processID;
    int clientSocket;
    sockaddr_in servAddress;
    std::atomic<bool> connected;
    std::function<void(Packet &)> passPacketCom;
    ISocket* socketInterface;
public:
    // Constructor
    ClientConnection(std::function<void(Packet &)> callback, ISocket* socketInterface = new RealSocket());

    // Function to load the server configuration from a JSON file
    ErrorCode loadServerConfig(const std::string& filePath);
    
    // Requesting a connection to the server
    ErrorCode connectToServer(uint32_t processID);

    // Sends the packet to the manager-sync
    ErrorCode sendPacket(const Packet &packet);

    // Waits for a message and forwards it to Communication
    void receivePacket();

    // Closes the connection
    ErrorCode closeConnection();

    // Setter for passPacketCom
    void setCallback(const std::function<void(Packet&)> callback);
    
    // Setter for socketInterface
    void setSocketInterface(ISocket* socketInterface);

    // For testing
    int getClientSocket() const;
    
    int isConnected() const;
    
    bool isReceiveThreadRunning() const;

    //Destructor
    ~ClientConnection();
};