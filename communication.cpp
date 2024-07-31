#include "communication.h"
#include <cstring>
#include <iostream>
#include <thread>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <iomanip>

const size_t communication::PACKET_SIZE = 16;
const std::string LOG_FILE = "communication.log";

void logMessage(const std::string& src, const std::string& dst, const std::string& message) {
    std::ofstream logFile(LOG_FILE, std::ios_base::app);
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time);

    logFile << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] "
            << "SRC: " << src << " DST: " << dst << " " << message << std::endl;
}

void communication::receiveMessages(int socketFd) 
{
    uint8_t buffer[PACKET_SIZE] = {0};
    std::cout << "[INFO] Entering receiveMessages function" << std::endl;
    logMessage("Server", "Client", "[INFO] Entering receiveMessages function");

    while (true) {
        int valread = recv(socketFd, buffer, PACKET_SIZE, 0);
        if (valread <= 0) {
            std::cerr << "[ERROR] Connection closed or error occurred" << std::endl;
            logMessage("Server", "Client", "[ERROR] Connection closed or error occurred");
            break;
        }

        std::ostringstream receivedPacket;
        receivedPacket << "[INFO] Received packet: ";
        for (int i = 0; i < valread; ++i) {
            receivedPacket << static_cast<int>(buffer[i]) << " ";
        }
        std::cout << receivedPacket.str() << std::endl;
        logMessage("Server", "Client", receivedPacket.str());
    }
}

void communication::sendMessages(int socketFd, void* data, size_t dataLen) 
{
    uint8_t buffer[PACKET_SIZE] = {0};
    size_t offset = 0;
    std::cout << "[INFO] Entering sendMessages function" << std::endl;
    logMessage("Client", "Server", "[INFO] Entering sendMessages function");

    while (offset < dataLen) {
        size_t packetLen = std::min(dataLen - offset, PACKET_SIZE);
        std::memcpy(buffer, static_cast<uint8_t*>(data) + offset, packetLen);
        if (send(socketFd, buffer, packetLen, 0) == -1) {
            std::cerr << "[ERROR] Failed to send packet" << std::endl;
            logMessage("Client", "Server", "[ERROR] Failed to send packet");
            break;
        }
        std::string message = "[INFO] Sent packet of size: " + std::to_string(packetLen);
        std::cout << message << std::endl;
        logMessage("Client", "Server", message);
        offset += packetLen;
    }
}

int communication::initConnection(int portNumber) {
    int peerPort = (portNumber == PORT1) ? PORT2 : PORT1;

    int sockFd, newSocket;
    struct sockaddr_in address, peerAddr;
    int opt = 1;
    int addrlen = sizeof(address);

    std::cout << "[INFO] Initializing connection on port " << portNumber << std::endl;
    logMessage("System", "System", "[INFO] Initializing connection on port " + std::to_string(portNumber));

    if ((sockFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("[ERROR] Socket creation error");
        logMessage("System", "System", "[ERROR] Socket creation error");
        return -1;
    }

    if (setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("[ERROR] setsockopt");
        logMessage("System", "System", "[ERROR] setsockopt");
        close(sockFd);
        return -1;
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(portNumber);

    if (bind(sockFd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("[ERROR] Bind failed");
        logMessage("System", "System", "[ERROR] Bind failed");
        close(sockFd);
        return -1;
    }

    if (listen(sockFd, 3) < 0) {
        perror("[ERROR] Listen failed");
        logMessage("System", "System", "[ERROR] Listen failed");
        close(sockFd);
        return -1;
    }

    std::cout << "[INFO] Listening on port " << portNumber << std::endl;
    logMessage("System", "System", "[INFO] Listening on port " + std::to_string(portNumber));

    memset(&peerAddr, 0, sizeof(peerAddr));
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(peerPort);

    if (inet_pton(AF_INET, "127.0.0.1", &peerAddr.sin_addr) <= 0) {
        perror("[ERROR] Invalid address/ Address not supported");
        logMessage("System", "System", "[ERROR] Invalid address/ Address not supported");
        return -1;
    }

    int clientSock = -1;
    std::thread recvThread;
    if (portNumber == PORT1) {
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Waiting for the other process to listen
        clientSock = socket(AF_INET, SOCK_STREAM, 0);
        if (connect(clientSock, (struct sockaddr*)&peerAddr, sizeof(peerAddr)) < 0) {
            perror("[ERROR] Connection Failed");
            logMessage("Client", "Server", "[ERROR] Connection Failed");
            return -1;
        }
        newSocket = accept(sockFd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (newSocket < 0) {
            perror("[ERROR] Accept failed");
            logMessage("Server", "Client", "[ERROR] Accept failed");
            return -1;
        }
        recvThread = std::thread(&communication::receiveMessages, this, newSocket);
    } else {
        newSocket = accept(sockFd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (newSocket < 0) {
            perror("[ERROR] Accept failed");
            logMessage("Server", "Client", "[ERROR] Accept failed");
            return -1;
        }
        recvThread = std::thread(&communication::receiveMessages, this, newSocket);
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Waiting for the server to listen
        clientSock = socket(AF_INET, SOCK_STREAM, 0);
        if (connect(clientSock, (struct sockaddr*)&peerAddr, sizeof(peerAddr)) < 0) {
            perror("[ERROR] Connection Failed");
            logMessage("Client", "Server", "[ERROR] Connection Failed");
            return -1;
        }
    }

    recvThread.detach();
    std::cout << "[INFO] Connection initialized successfully" << std::endl;
    logMessage("System", "System", "[INFO] Connection initialized successfully");
    return clientSock;
    
}
