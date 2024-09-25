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

// Name of the shared memory and semaphores
#define START_SEM_NAME "/start_sem"
#define SHM_NAME "/shared_memory"
#define PROCESSES_COUNT 100

// Data structure to be stored in shared memory
struct SharedData {
    std::atomic<int> registeredCriticalProcesses;  
    std::atomic<int> criticalProcessesCount;
    int maxCriticalProcessID;
    std::atomic<int> registeredProcessesCount;
    int processCount;          // Variable holding the actual number of processes
    uint32_t processIds[PROCESSES_COUNT];
};

// SyncCommunication class definition
class SyncCommunication
{
public: 
    // Default constructor - for other processes
    SyncCommunication();

    ~SyncCommunication();

    static void close();

    static void registerProcess(uint32_t processId);

    static void initializeManager(const std::vector<uint32_t>& processIds, int maxCriticalProcessID);

    static void initializeProcess();
    
private:
    // Variables
    static SharedData* sharedData;
    static sem_t* startSem;
    static timer_t timerID;
    struct itimerspec its;
    
    // Static function to handle wait timeout
    static void handle_timeout(int signum);
    
    // Setting timeout
    static const int timeout = 10; // Seconds
};

#endif // SYNS_COMMUNICATION_H