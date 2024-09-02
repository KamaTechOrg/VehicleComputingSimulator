#include "synsCommunication.h"

SynsCommunication::SynsCommunication()
{
}

SynsCommunication::SynsCommunication(int numProcesses,int maxCriticalProcessID)
    : expectedProcesses(numProcesses),maxCriticalProcessID(maxCriticalProcessID)
    {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(int));
    registeredProcesses = static_cast<int*>(mmap(0, sizeof(int), PROT_WRITE, MAP_SHARED, shm_fd, 0));
    *registeredProcesses = 0;

    sem = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    startSem = sem_open(START_SEM_NAME, O_CREAT, 0644, 0);
    if (startSem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    managerSem = sem_open(MANAGER_SEM_NAME, O_CREAT, 0644, 0);
    if (managerSem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
}

SynsCommunication::~SynsCommunication()
{
    sem_close(sem);
    sem_unlink(SEM_NAME);

    sem_close(startSem);
    sem_unlink(START_SEM_NAME);

    sem_close(managerSem);
    sem_unlink(MANAGER_SEM_NAME);

    munmap(registeredProcesses, sizeof(int));
    shm_unlink(SHM_NAME);
}

void SynsCommunication::isManagerRunning()
{
    sem_wait(managerSem);
}

void SynsCommunication::registerProcess(int processId)
{
    sem_wait(sem);
    if(processId < maxCriticalProcessID)
        ++(*registeredProcesses);
    sem_post(sem);

    if (*registeredProcesses == expectedProcesses) {
        for (int i = 0; i < expectedProcesses; ++i){
            sem_post(startSem);
        }
    }else{
    sem_wait(startSem);
    }
}

void SynsCommunication::initializeManager()
{
    sem_post(managerSem);
}

