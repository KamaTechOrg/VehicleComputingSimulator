#include <vector>
#include <thread>
#include <mutex>
#include <netinet/in.h>
#include <unistd.h>
#include "message.hpp"
#include <functional>
#include <map>
#include "logger.hpp"

// If BROADCAST_MODE is defined, broadcast to all clients, otherwise send to specific ID
// #define BROADCAST_MODE

class Server
{
private:
    int serverSocket;
    sockaddr_in address;
    int port;
    bool running;
    std::vector<std::thread> clientThreads;
    std::vector<int> sockets;
    std::mutex socketMutex;
    std::function<void(void *)> receiveData;
    std::map<int, uint32_t> clientIDMap;

public:
    // constructor
    Server(int port, std::function<void(void *)> callback);
    
    // Starts listening for connection requests
    int start();
    
    // Closes the sockets and the treads
    void stop();
    
    // Runs in a thread for each process - waits for a message and forwards it to the manager
    void handleClient(int clientSocket);
    
     // Returns the sockets ID
    int getClientSocketByID(uint32_t destID);
    
    // Sends the message to all connected processes-broudcast
    int sendToClients(const Packet &packet);
    
};