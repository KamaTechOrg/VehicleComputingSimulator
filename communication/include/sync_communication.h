#ifndef SYNS_COMMUNICATION_H
#define SYNS_COMMUNICATION_H

#include <atomic>
#include <vector>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <algorithm>
#include <cstring>
#include <mutex>
#include <condition_variable>
#include "error_code.h"

class SyncCommunication
{
public: 
    SyncCommunication();

    ~SyncCommunication();

    static ErrorCode registerProcess(uint32_t processId);

    static ErrorCode initializeManager(const std::vector<uint32_t>& processIds, int maxCriticalProcessID);

    static void handle_timeout(int signum);
    
    static bool critical_ready;

    static std::mutex mutexCV;

    static std::condition_variable cv;
    
private:
    static timer_t timerID;
    struct itimerspec its;
    // Data structure to be stored
    static std::atomic<int> registeredCriticalProcesses;  
    static std::atomic<int> criticalProcessesCount;
    static int maxCriticalProcessID;
    static std::atomic<int> registeredProcessesCount;
    static std::vector<uint32_t> processIDs;
    static const int timeout = 10;
};

#endif // SYNS_COMMUNICATION_H