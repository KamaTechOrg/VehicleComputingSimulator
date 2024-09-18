#include "../include/central_manager.h"
#include <iostream>
#include <stdexcept>

CentralManager* CentralManager::instance = nullptr;

// Singleton instance retrieval
CentralManager* CentralManager::getInstance(std::vector<int> ports)
{
    if (instance == nullptr) 
        instance = new CentralManager(ports);
    
    return instance;
}

// Singleton pattern: private constructor
CentralManager::CentralManager(std::vector<int> ports) : ports(ports)
{
    for (auto port : ports)
        managers[port] = new BusManager(port, std::bind(&CentralManager::receiveMessage, this, std::placeholders::_1), std::bind(&CentralManager::receiveNewProcessID, this, std::placeholders::_1, std::placeholders::_2));
}

// Start the connection for the central manager
ErrorCode CentralManager::startConnection()
{
    ErrorCode allProcessConnected = ErrorCode::SUCCESS;
    std::lock_guard<std::mutex> lock(managersMutex);
    for (auto& [port, manager] : managers) {
        ErrorCode code = manager->startConnection();
        if (code != ErrorCode::SUCCESS)
            allProcessConnected = code;
    }
    
    return allProcessConnected;
}

// Check if a new bus can connect while running
// Register a new bus manager for the given processId and route
ErrorCode CentralManager::registerBusManager(const uint32_t port)
{
    std::lock_guard<std::mutex> lock(managersMutex);
    if (managers.find(port) != managers.end())
        return ErrorCode::CONNECTION_FAILED;
    
    BusManager* newManager = new BusManager(port, std::bind(&CentralManager::receiveMessage, this, std::placeholders::_1), std::bind(&CentralManager::receiveNewProcessID, this, std::placeholders::_1, std::placeholders::_2));
    ErrorCode code = newManager->startConnection();
    if (code != ErrorCode::SUCCESS)
        return code;

    managers[port] = newManager;
    return ErrorCode::SUCCESS;      
}

ErrorCode CentralManager::receiveNewProcessID(const uint32_t processID, const uint32_t port)
{
    {
        std::lock_guard<std::mutex> lock(managersMutex);
         if (managers.find(port) == managers.end())
            return ErrorCode::BUS_NOT_FOUND;
    }
    {
        std::lock_guard<std::mutex> lock(processToPortMutex);
        if (processToPort.find(processID) != processToPort.end())
            return ErrorCode::INVALID_ID;
        processToPort[processID] = port;
    } 
    return ErrorCode::SUCCESS;
}

// Receive a message from a specific bus and decide what to do with it
ErrorCode CentralManager::receiveMessage(Packet& packet)
{
    if (needsTranslation(packet.header.SrcID, packet.header.DestID))
        forwardToGateway(packet);
    return sendMessage(packet);
}

// Close the connection for the central manager
ErrorCode CentralManager::closeConnection()
{
    ErrorCode allProcessClosed = ErrorCode::SUCCESS;
    std::lock_guard<std::mutex> lock(managersMutex);
    for (auto& [id, manager] : managers) {
        ErrorCode code = manager->closeConnection();
        if (code != ErrorCode::SUCCESS)
            allProcessClosed = code;
    }

    return allProcessClosed;
}

// Send a message to a specific bus
ErrorCode CentralManager::sendMessage(const Packet& packet)
{
    uint32_t destID = packet.header.DestID, destPort;
    BusManager* destBus;
    {
        std::lock_guard<std::mutex> lock(processToPortMutex);
        // Find the manager of destID
        if (processToPort.find(destID) == processToPort.end())
            return ErrorCode::INVALID_ID;

        destPort = processToPort[destID];
    }
    {
        std::lock_guard<std::mutex> managersLock(managersMutex);
        if (managers.find(destPort) == managers.end())
            return ErrorCode::BUS_NOT_FOUND;

        destBus = managers[destPort];
    }
    
    return destBus->sendMessage(packet);
}

// Forward the packet to the gateway for protocol translation
ErrorCode CentralManager::forwardToGateway(Packet& packet)
{
    std::lock_guard<std::mutex> lock(gatewayMutex);
    if (gateway) {
        gateway->translate(packet);
        return ErrorCode::SUCCESS;
    } else
        return ErrorCode::BUS_NOT_FOUND;
}

// Check if a packet needs protocol translation
bool CentralManager::needsTranslation(uint32_t srcId, uint32_t dstId) {
    // Implement protocol checking
    return false;
}
