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
#include "error_code.h"

#define PORT 80
#define IP "213.151.44.47"
// #define IP "192.168.33.14"
// #define IP "100.87.44.47"
// #define IP "127.0.0.1"
// #define IP "https://699a9ee344d0a2963ff3324740836bac.serveo.net"
class ClientConnection
{
private:
    int clientSocket;
    sockaddr_in servAddress;
    std::atomic<bool> connected;
    std::function<void(const Packet &)> passPacketCom;
    ISocket* socketInterface;
    std::thread receiveThread;
    uint16_t port;
public:
    // Constructor
    ClientConnection(std::function<void(const Packet &)> callback, ISocket* socketInterface = new RealSocket());

    // Requesting a connection to the server
    ErrorCode connectToServer(uint16_t port, int id);

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

