#include "../include/central_manager.h"
#include <iostream>
#include <stdexcept>
#include "../include/central_manager.h"
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <string>

CentralManager* CentralManager::instance = nullptr;
std::vector<uint16_t> CentralManager::ports;

// Singleton instance retrieval
CentralManager* CentralManager::getInstance()
{   
    if (instance == nullptr)
        instance = new CentralManager();
    
    return instance;
}

// Singleton pattern: private constructor
CentralManager::CentralManager() : running(false)
{
    ErrorCode res = NetworkInfo::loadProcessConfig("../config.json");
    if (res != ErrorCode::SUCCESS) {
        std::string errorMsg = "Failed to load process configuration from ../config.json. Error code: " + std::to_string(static_cast<int>(res));
        RealSocket::log.logMessage(logger::LogLevel::ERROR, errorMsg);
        throw std::runtime_error(errorMsg);
    }

    ports = NetworkInfo::getPorts();
    processIds = NetworkInfo::getProcessIds();
    maxCriticalProcessID = NetworkInfo::getMaxCriticalProcessID();
    
    // Setup the signal handler for SIGINT
    signal(SIGINT, CentralManager::signalHandler);
    SyncCommunication::initializeManager(processIds, maxCriticalProcessID);

    for (auto port : CentralManager::ports)
        managers[port] = new BusManager(port, std::bind(&CentralManager::receiveMessage, this, std::placeholders::_1), std::bind(&CentralManager::updateProcessID, this, std::placeholders::_1, std::placeholders::_2,std::placeholders::_3));
}

// Start the connection for the central manager
ErrorCode CentralManager::startConnection()
{
    running.exchange(true);
    ErrorCode allProcessConnected = ErrorCode::SUCCESS;
    std::lock_guard<std::mutex> lock(managersMutex);

    for (auto& [port, manager] : managers) {
        ErrorCode code = manager->startConnection();
        if (code != ErrorCode::SUCCESS){
            RealSocket::log.logMessage(logger::LogLevel::ERROR, "Central Manager : Error connecting port: " + std::to_string(port) + "  " + toString(code));
            allProcessConnected = code;
        }
        RealSocket::log.logMessage(logger::LogLevel::INFO, "Central Manager : Bus is running on port: " + std::to_string(port));
    }
    
    return allProcessConnected;
}

// Check if a new bus can connect while running
// Register a new bus manager for the given processId and route
ErrorCode CentralManager::registerBusManager(const uint16_t port)
{
    std::lock_guard<std::mutex> lock(managersMutex);
    if (managers.find(port) != managers.end())
        return ErrorCode::CONNECTION_FAILED;
    
    BusManager* newManager = new BusManager(port, std::bind(&CentralManager::receiveMessage, this, std::placeholders::_1), std::bind(&CentralManager::updateProcessID, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    ErrorCode code = newManager->startConnection();
    if (code != ErrorCode::SUCCESS){
        RealSocket::log.logMessage(logger::LogLevel::ERROR, "Central Manager : Error connecting port: " + std::to_string(port) + "  " + toString(code));
        return code;
    }

    managers[port] = newManager;
    RealSocket::log.logMessage(logger::LogLevel::INFO, "Central Manager : Bus is running on port: " + std::to_string(port));
    return ErrorCode::SUCCESS;      
}

ErrorCode CentralManager::updateProcessID(const uint32_t processID, const uint16_t port, bool isConnected)
{
    if(!running.load())
        return ErrorCode::NOT_RUNNING;
    {
        std::lock_guard<std::mutex> lock(managersMutex);
         if (managers.find(port) == managers.end()){
            RealSocket::log.logMessage(logger::LogLevel::ERROR, "Central Manager : Port: " + std::to_string(port) + " not found for process: " + std::to_string(processID));
            return ErrorCode::BUS_NOT_FOUND;
         }
    }
    {
        std::lock_guard<std::mutex> lock(processToPortMutex);
        auto it = processToPort.find(processID);
        if ( it != processToPort.end()){
            if (!isConnected){
                processToPort.erase(it);
                RealSocket::log.logMessage(logger::LogLevel::INFO, "Central Manager : Process has disconnected: " + std::to_string(processID));
                return ErrorCode::SUCCESS;
            } 
            RealSocket::log.logMessage(logger::LogLevel::ERROR, "Central Manager : Invalid ID for connection: " + std::to_string(processID));
            return ErrorCode::INVALID_ID;
        }
        processToPort[processID] = port;
        return SyncCommunication::registerProcess(processID);
    }
    RealSocket::log.logMessage(logger::LogLevel::INFO, "Central Manager : Process has connected: " + std::to_string(processID));
    return ErrorCode::SUCCESS;
}

// Receive a message from a specific bus and decide what to do with it
ErrorCode CentralManager::receiveMessage(Packet& packet)
{
    if (needsTranslation(packet.header.SrcID, packet.header.DestID))
        forwardToGateway(packet);
    return sendMessage(packet);
}

// Send a message to a specific bus
ErrorCode CentralManager::sendMessage(const Packet& packet)
{
    uint32_t destID = packet.header.DestID, destPort, srcPort;
    BusManager* destBus;
    {
        std::lock_guard<std::mutex> lock(processToPortMutex);
        // Find the manager of destID
        if (processToPort.find(destID) == processToPort.end()){
            RealSocket::log.logMessage(logger::LogLevel::ERROR,std::to_string(packet.header.SrcID), std::to_string(packet.header.DestID), "Central Manager : destID not found: " + std::to_string(destID));
            return ErrorCode::INVALID_ID;
        }
        
        destPort = processToPort[destID];
        srcPort = processToPort[packet.header.SrcID];
    }
    {
        std::lock_guard<std::mutex> managersLock(managersMutex);
        if (managers.find(destPort) == managers.end()){
            RealSocket::log.logMessage(logger::LogLevel::ERROR,std::to_string(packet.header.SrcID), std::to_string(packet.header.DestID), "Central Manager : destPort not found: " + std::to_string(destPort));
            return ErrorCode::BUS_NOT_FOUND;
        }
        
        destBus = managers[destPort];
    }

    RealSocket::log.logMessage(logger::LogLevel::INFO, std::to_string(packet.header.SrcID), std::to_string(packet.header.DestID), "Central Manager : On bus "+ std::to_string(srcPort) +" Passing to bus: " + std::to_string(destPort));
    return destBus->sendMessage(packet);
}

// Forward the packet to the gateway for protocol translation
ErrorCode CentralManager::forwardToGateway(Packet& packet)
{
    std::lock_guard<std::mutex> lock(gatewayMutex);
    if (gateway) {
        gateway->translate(packet);
        RealSocket::log.logMessage(logger::LogLevel::INFO,std::to_string(packet.header.SrcID), std::to_string(packet.header.DestID), "Central Manager : translate packet succeeded ");
        return ErrorCode::SUCCESS;
    } else
        RealSocket::log.logMessage(logger::LogLevel::ERROR,std::to_string(packet.header.SrcID), std::to_string(packet.header.DestID), "Central Manager : translate packet failed ");
        return ErrorCode::BUS_NOT_FOUND;
}

// Check if a packet needs protocol translation
bool CentralManager::needsTranslation(uint32_t srcId, uint32_t dstId) {
    // Implement protocol checking
    return false;
}

// Close the connection for the central manager
ErrorCode CentralManager::closeConnection()
{
    running.exchange(false);
    ErrorCode allProcessClosed = ErrorCode::SUCCESS;
    std::lock_guard<std::mutex> lock(managersMutex);
    for (auto& [id, manager] : managers) {
        ErrorCode code = manager->closeConnection();
        if (code != ErrorCode::SUCCESS)
            allProcessClosed = code;
    }
    RealSocket::log.logMessage(logger::LogLevel::INFO, "Central Manager : Close connection succeeded");
    RealSocket::log.cleanUp();

    return allProcessClosed;
}

// Static method to handle SIGINT signal
void CentralManager::signalHandler(int signum)
{
    if (instance)
        instance->closeConnection();

    exit(signum);
}

// Destructor
CentralManager::~CentralManager()
{
    closeConnection();
    for (auto& [port, manager] : managers)
        delete manager; 

}
