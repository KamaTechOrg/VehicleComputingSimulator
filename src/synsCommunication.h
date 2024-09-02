
#ifndef SYNC_COMMUNICATION_H
#define SYNC_COMMUNICATION_H

#include <semaphore.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstdio>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

class SynsCommunication
{
public:
    SynsCommunication();
    SynsCommunication(int numProcesses,int maxCriticalProcessID);
    ~SynsCommunication();

    void registerProcess(int processId);
    void isManagerRunning();
    void initializeManager();
private:
    int expectedProcesses;
    int maxCriticalProcessID;
    int* registeredProcesses;
    sem_t* sem;
    sem_t* startSem;
    sem_t* managerSem;
    const char* SEM_NAME = "/process_sync_sem";
    const char* START_SEM_NAME = "/start_sync_sem";
    const char* MANAGER_SEM_NAME = "/manager_sync_sem";
    const char* SHM_NAME = "/process_sync_shm";
};

#endif
