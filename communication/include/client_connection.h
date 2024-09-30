#pragma once
#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <functional>
#include <iostream>
#include "message.h"
#include "../sockets/Isocket.h"
#include "../sockets/mock_socket.h"
#include "../sockets/real_socket.h"
#include <string>
#include <nlohmann/json.hpp>
#include "error_code.h"

class ClientConnection
{
private:
    const char* serverIP;
    uint16_t serverPort;
    uint32_t processID;
    int clientSocket;
    sockaddr_in servAddress;
    std::atomic<bool> connected;
    std::function<void(const Packet &)> passPacketCom;
    ISocket* socketInterface;
    std::thread receiveThread;
public:
    // Constructor
    ClientConnection(std::function<void(const Packet &)> callback, ISocket* socketInterface = new RealSocket());

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
    void setCallback(const std::function<void(const Packet&)> callback);
    
    // Setter for socketInterface
    void setSocketInterface(ISocket* socketInterface);

    // For testing
    int getClientSocket() const;
    
    int isConnected() const;
    
    bool isReceiveThreadRunning() const;

    //Destructor
    ~ClientConnection();
};

