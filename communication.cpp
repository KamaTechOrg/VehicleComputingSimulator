#include "communication.h"
#include <cstring>
#include <iostream>
#include <thread>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm>
#include <fstream>

// Constants for packet size and state file names
const size_t communication::PACKET_SIZE = 16;
const char* communication::STATE_FILE = "comm_state3.txt";
const char* communication::LOCK_FILE = "comm_state3.lock";
std::mutex communication::state_file_mutex;

// Function to receive messages from a socket
void communication::receiveMessages(int socketFd)
{
    uint8_t buffer[PACKET_SIZE] = {0};
    std::cout << "before the while in receive func" << std::endl; // Before entering the loop in the receive function
    while (true) {
        int valread = recv(socketFd, buffer, PACKET_SIZE, 0);
        if (valread <= 0) {
            std::cerr << "Connection closed or error occurred" << std::endl; // Connection closed or an error occurred
            break;
        }

        std::cout << "Received packet: ";
        for (int i = 0; i < valread; ++i) {
            std::cout << static_cast<int>(buffer[i]) << " ";
        }
        std::cout << std::endl;
    }
}

// Function to send messages to a socket
void communication::sendMessages(int socketFd, void* data, size_t dataLen)
{
    uint8_t buffer[PACKET_SIZE] = {0};
    size_t offset = 0;
    std::cout << "in send func" << std::endl; // Inside the send function
    while (offset < dataLen) {
        size_t packetLen = std::min(dataLen - offset, PACKET_SIZE);
        std::memcpy(buffer, static_cast<uint8_t*>(data) + offset, packetLen);
        send(socketFd, buffer, packetLen, 0);
        offset += packetLen;
    }
}


// Function to initialize the state based on the presence of the state file
void communication::initializeState(int& portNumber, int& peerPort) 
{
    std::lock_guard<std::mutex> lock(state_file_mutex);
    std::ifstream infile(STATE_FILE);
    if (!infile) {  // If the file doesn't exist - means this is the first process
        portNumber = PORT2;
        peerPort = PORT1;
        std::ofstream outfile(STATE_FILE);
        outfile << "initialized";
        outfile.close();  
        std::ofstream lockFile(LOCK_FILE); 
        lockFile.close();
    } else {  // If the file already exists - means this is the second process
        portNumber = PORT1;
        peerPort = PORT2;
    }
    infile.close();
}

// Function to set up a socket for listening
int communication::setupSocket(int portNumber, int& sockFd, struct sockaddr_in& address) 
{
    int opt = 1;
    if ((sockFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    if (setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(sockFd);
        return -1;
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(portNumber);

    if (bind(sockFd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(sockFd);
        return -1;
    }

    if (listen(sockFd, 3) < 0) {
        perror("Listen");
        close(sockFd);
        return -1;
    }

    std::cout << "Listening on port " << portNumber << std::endl;
    return 0;
}

// Function to wait for an incoming connection
int communication::waitForConnection(int sockFd, struct sockaddr_in& address) 
{
    int newSocket;
    int addrlen = sizeof(address);
    if ((newSocket = accept(sockFd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        return -1;
    }
    return newSocket;
}

// Function to connect to a peer socket
int communication::connectToPeer(int portNumber, struct sockaddr_in& peerAddr) 
{
    int clientSock = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSock < 0) {
        perror("Socket creation error");
        return -1;
    }

    if (inet_pton(AF_INET, "127.0.0.1", &peerAddr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    if (connect(clientSock, (struct sockaddr*)&peerAddr, sizeof(peerAddr)) < 0) {
        perror("Connection Failed");
        return -1;
    }

    return clientSock;
}

// Function to initialize the connection
int communication::initConnection() 
{
    int portNumber, peerPort;
    initializeState(portNumber, peerPort);

    int sockFd, newSocket;
    struct sockaddr_in address, peerAddr;

    if (setupSocket(portNumber, sockFd, address) < 0) {
        return -1;
    }

    memset(&peerAddr, 0, sizeof(peerAddr));
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(peerPort);

    int clientSock = -1;
    std::thread recvThread;

    if (portNumber == PORT1) {
        // Wait briefly to ensure the second process (PORT2) starts listening
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        clientSock = connectToPeer(portNumber, peerAddr);
        if (clientSock < 0) return -1; 

        // Wait for an incoming connection from PORT2
        newSocket = waitForConnection(sockFd, address);
        if (newSocket < 0) return -1; 
        
        // Start a separate thread to handle incoming messages on the new socket
        recvThread = std::thread(&communication::receiveMessages, this, newSocket);
    } else {
       
        // Wait for an incoming connection from PORT1
        newSocket = waitForConnection(sockFd, address);
        if (newSocket < 0) return -1; 
        
        // Start a separate thread to handle incoming messages on the new socket
        recvThread = std::thread(&communication::receiveMessages, this, newSocket);
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Waiting for the server to listen
        
        clientSock = connectToPeer(portNumber, peerAddr);
        if (clientSock < 0) return -1; 

        // Clean up: remove state and lock files as they are no longer needed
        std::remove(STATE_FILE);
        std::remove(LOCK_FILE);
    }

    recvThread.detach();
    std::cout << "Connection initialized" << std::endl;
    return clientSock;
}
