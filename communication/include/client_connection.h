// #pragma once
// #include <thread>
// #include <netinet/in.h>
// #include <unistd.h>
// #include <arpa/inet.h>
// #include <functional>
// #include <iostream>
// #include <string>
// #include "message.h"
// #include "../sockets/Isocket.h"
// #include "../sockets/ESP32_socket.h"
// #include "../sockets/real_socket.h"
// #include "error_code.h"

// #define PORT 8080
// #define IP "172.28.151.112"

// class ClientConnection
// {
// private:
//     int clientSocket;
//     sockaddr_in servAddress;
//     std::atomic<bool> connected;
//     std::function<void(Packet &)> passPacketCom;
//     ISocket* socketInterface;
//     std::thread receiveThread;

// public:
//     // Constructor
//     ClientConnection(std::function<void(Packet &)> callback, ISocket* socketInterface);

//     // Requesting a connection to the server
//     ErrorCode connectToServer(int id);

//     // Sends the packet to the manager-sync
//     ErrorCode sendPacket(Packet &packet);

//     // Waits for a message and forwards it to Communication
//     void receivePacket();

//     // Closes the connection
//     ErrorCode closeConnection();

//     // Setter for passPacketCom
//     void setCallback(std::function<void(Packet&)> callback);
    
//     // Setter for socketInterface
//     void setSocketInterface(ISocket* socketInterface);

//     // For testing
//     int getClientSocket();
    
//     int isConnected();
    
//     bool isReceiveThreadRunning();

//     //Destructor
//     ~ClientConnection();
// };
#pragma once

#ifdef INADDR_NONE
#undef INADDR_NONE
#endif

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
#include <atomic>
#include <string>
#include "message.h"
#include "../sockets/Isocket.h"
#include "error_code.h"
#include "real_socket.h"
#endif

#define PORT 8080
#define IP "172.28.151.112"

class ClientConnection
{
private:
#ifdef ESP32
    TaskHandle_t receiveTaskHandle;
#else
    std::thread receiveThread;
#endif
    int clientSocket;
    sockaddr_in servAddress;
    std::atomic<bool> connected;
    std::function<void(Packet &)> passPacketCom;
    ISocket* socketInterface;

public:
    ClientConnection(std::function<void(Packet &)> callback, ISocket* socketInterface = nullptr);
    // Requesting a connection to the server
    ErrorCode connectToServer(int id);

    // Sends the packet to the manager-sync
    ErrorCode sendPacket(Packet &packet);

    // Waits for a message and forwards it to Communication
    void receivePacket();

    // Closes the connection
    ErrorCode closeConnection();

    // Setter for passPacketCom
    void setCallback(std::function<void(Packet&)> callback);
    
    // Setter for socketInterface
    void setSocketInterface(ISocket* socketInterface);

    // For testing
    int getClientSocket();
    
    int isConnected();
    
    bool isReceiveThreadRunning();

    //Destructor
    ~ClientConnection();
};