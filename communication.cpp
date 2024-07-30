#include "communication.h"
#include <cstring>
#include <iostream>
#include <thread>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm>
#include <fstream>

const size_t communication::PACKET_SIZE = 16;
const char* communication::STATE_FILE = "comm_state3.txt";
const char* communication::LOCK_FILE = "comm_state3.lock";
std::mutex communication::state_file_mutex;

void communication::receiveMessages(int socketFd) 
{
    uint8_t buffer[PACKET_SIZE] = {0};
    std::cout << "before the while in receive func" << std::endl;
    while (true) {
        int valread = recv(socketFd, buffer, PACKET_SIZE, 0);
        if (valread <= 0) {
            std::cerr << "Connection closed or error occurred" << std::endl;
            break;
        }

        std::cout << "Received packet: ";
        for (int i = 0; i < valread; ++i) {
            std::cout << static_cast<int>(buffer[i]) << " ";
        }
        std::cout << std::endl;
    }
}

void communication::sendMessages(int socketFd, void* data, size_t dataLen) 
{
    uint8_t buffer[PACKET_SIZE] = {0};
    size_t offset = 0;
    std::cout << "in send func" << std::endl;
    while (offset < dataLen) {
        size_t packetLen = std::min(dataLen - offset, PACKET_SIZE);
        std::memcpy(buffer, static_cast<uint8_t*>(data) + offset, packetLen);
        send(socketFd, buffer, packetLen, 0);
        offset += packetLen;
    }
}

int communication::initConnection() {
    int portNumber, peerPort;

    std::lock_guard<std::mutex> lock(state_file_mutex);
    std::ifstream infile(STATE_FILE);
    if (!infile) {  // אם הקובץ לא קיים
        portNumber = PORT2;
        peerPort = PORT1;
        std::ofstream outfile(STATE_FILE);
        outfile << "initialized";
        outfile.close();  // יוצר את הקובץ ומעדכן את המצב
        std::ofstream lockFile(LOCK_FILE); // יוצר את קובץ הנעילה
        lockFile.close();
    } else {  // אם הקובץ כבר קיים
        portNumber = PORT1;
        peerPort = PORT2;
    }
    infile.close();

    int sockFd, newSocket;
    struct sockaddr_in address, peerAddr;
    int opt = 1;
    int addrlen = sizeof(address);

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

    memset(&peerAddr, 0, sizeof(peerAddr));
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(peerPort);

    if (inet_pton(AF_INET, "127.0.0.1", &peerAddr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    int clientSock = -1;
    std::thread recvThread;
    if (portNumber == PORT1) {
        std::this_thread::sleep_for(std::chrono::seconds(1)); // מחכים שהתהליך השני יאזין
        clientSock = socket(AF_INET, SOCK_STREAM, 0);
        if (connect(clientSock, (struct sockaddr*)&peerAddr, sizeof(peerAddr)) < 0) {
            perror("Connection Failed");
            return -1;
        }
        newSocket = accept(sockFd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (newSocket < 0) {
            perror("Accept failed");
            return -1;
        }
        recvThread = std::thread(&communication::receiveMessages, this, newSocket);
    } else {
        newSocket = accept(sockFd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (newSocket < 0) {
            perror("Accept failed");
            return -1;
        }
        recvThread = std::thread(&communication::receiveMessages, this, newSocket);
        std::this_thread::sleep_for(std::chrono::seconds(1)); // מחכים שהשרת יאזין
        clientSock = socket(AF_INET, SOCK_STREAM, 0);
        if (connect(clientSock, (struct sockaddr*)&peerAddr, sizeof(peerAddr)) < 0) {
            perror("Connection Failed");
            return -1;
        }

        // לאחר קבלת פורט 8080, נמחק את הקובץ המשותף
        std::remove(STATE_FILE);
        std::remove(LOCK_FILE);
    }

    recvThread.detach();
    std::cout << "Connection initialized" << std::endl;
    return clientSock;
}

void communication::cleanUp() {
    std::lock_guard<std::mutex> lock(state_file_mutex);
    // אין צורך למחוק את הקובץ כאן שוב, זה מתבצע בתהליך השני
}
