#pragma once
#include <vector>
#include <thread>
#include <future>
#include <mutex>
#include <netinet/in.h>
#include <unistd.h>
#include <functional>
#include <unordered_map>
#include <algorithm>
#include <csignal>
#include <iostream>
#include "message.h"
#include "../sockets/Isocket.h"
#include "../sockets/real_socket.h"
#include "error_code.h"
#include "sync_communication.h"

class ServerConnection
{
private:
    int serverSocket;
    sockaddr_in address;
    uint16_t port;
    std::atomic<bool> running;
    std::thread mainThread;
    std::vector<std::thread> clientThreads;
    std::vector<int> sockets;
    std::mutex socketMutex;
    std::mutex threadMutex;
    std::function<ErrorCode(Packet&)> receiveDataCallback;
    std::function<ErrorCode(const uint32_t,const uint16_t, bool)> receiveNewProcessIDCallback;
    std::unordered_map<int, uint32_t> clientIDMap;
    std::mutex IDMapMutex;
    ISocket* socketInterface;

    // Starts listening for connection requests
    void startThread();

    // Implementation according to the CAN BUS
    bool isValidId(uint32_t id);
    
    // Runs in a thread for each process - waits for a message and forwards it to the manager
    void handleClient(int clientSocket);

    // Returns the sockets ID
    int getClientSocketByID(uint32_t destID);

public:

    // Constructor
    ServerConnection(uint16_t port, std::function<ErrorCode(Packet&)> receiveDataCallback,std::function<ErrorCode(const uint32_t,const uint16_t, bool)> receiveNewProcessIDCallback, ISocket* socketInterface = new RealSocket());
    
    // Initializes the listening socket
    ErrorCode startConnection();
    
    // Closes the sockets and the threads
    ErrorCode stopServer();

    // Sends the message to all connected processes - broadcast
    ErrorCode sendBroadcast(const Packet &packet);

    // Sets the server's port number, throws an exception if the port is invalid.
    void setPort(uint16_t port);

    // Sets the callback for receiving data, throws an exception if the callback is null.
    void setReceiveDataCallback(std::function<ErrorCode(Packet&)> callback);

    // Sets the socket interface, throws an exception if the socketInterface is null.
    void setSocketInterface(ISocket *socketInterface);              

    // Sends the message to destination
    ErrorCode sendDestination(const Packet &packet);
    
    // For testing
    int getServerSocket() const;

    int isRunning() const;

    const std::vector<int>* getSockets() const;

    const std::mutex* getSocketMutex() const;

    const std::mutex* getIDMapMutex() const;

    const std::unordered_map<int, uint32_t>* getClientIDMap() const;

    void testHandleClient(int clientSocket);

    int testGetClientSocketByID(uint32_t destID);

    // Destructor
     ~ServerConnection();
};