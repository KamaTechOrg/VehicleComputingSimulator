#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <cstddef>
#include <iostream>
#include <mutex>

class communication {
public:
    int initConnection();
    void sendMessages(int socketFd, void* data, size_t dataLen);
    void receiveMessages(int socketFd);
    static void cleanUp(); // הוספת הפונקציה לניקוי

private:
    const int PORT1 = 8080;
    const int PORT2 = 8081;
    static const size_t PACKET_SIZE;
    static const char* STATE_FILE;
    static const char* LOCK_FILE;
    static std::mutex state_file_mutex;
};

#endif
