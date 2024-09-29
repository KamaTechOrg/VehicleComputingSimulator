#ifndef CENTRALMANAGER_H
#define CENTRALMANAGER_H

#include <unordered_map>
#include <vector>
#include <string>
#include <cstdint>
#include <mutex>
#include "gateway.h"
#include "bus_manager.h"
#include "packet.h"
#include "error_code.h"
#include "Imanager.h"
#include "sync_communication.h"

class CentralManager:IManager{
private:
    static CentralManager* instance;
    static std::vector<uint16_t> ports;
    Gateway* gateway;
    std::atomic<bool> running;
    
    // Mutexes for protecting shared data
    std::mutex managersMutex;
    std::mutex processToPortMutex;
    std::mutex gatewayMutex;

    //private constructor to singleton
    CentralManager();

    //manager port - manager object
    std::unordered_map<uint16_t, BusManager*> managers;
    
    //process id - manager port
    std::unordered_map<uint32_t, uint16_t> processToPort;

    //check if packet need a convertion
    bool needsTranslation(uint32_t srcId, uint32_t dstId);

public:
    static CentralManager* getInstance();

    // Initializing the ports of the busses
    static std::unordered_map<uint32_t, uint16_t> assignPorts(std::vector<uint32_t>& ids);

    // Checking if the port is free
    static bool isPortAvailable(uint16_t port);

    //connect centeral manager
    ErrorCode startConnection() override;

    //close the centeral manager
    ErrorCode closeConnection() override;

    // Static method to handle SIGINT signal
    static void signalHandler(int signum);

    //recieve message from some bus
    ErrorCode receiveMessage(Packet &packet) override;

    //recieve processId from some bus
    ErrorCode updateProcessID(const uint32_t processID ,const uint16_t port, bool isConnected) override;

    //send message to some bus
    ErrorCode sendMessage(const Packet &packet) override;

    //convert the packet to another protocol
    ErrorCode forwardToGateway(Packet &packet);

    //connect new bus manager
    ErrorCode registerBusManager(const uint16_t port);

    ErrorCode notifyAllProcess();
    //Destructor
    ~CentralManager();
};

#endif // CENTRALMANAGER_H
