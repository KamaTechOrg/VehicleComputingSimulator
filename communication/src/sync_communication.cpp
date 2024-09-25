#include "../include/syncCommunication.h"

SharedData* SyncCommunication::sharedData;
sem_t* SyncCommunication::startSem;
timer_t SyncCommunication::timerID;

SyncCommunication::SyncCommunication()
{
}

void SyncCommunication::initializeManager(const std::vector<uint32_t>& processIds, int maxCriticalProcessID)
{
    // Initialize semaphore
    startSem = sem_open(START_SEM_NAME, O_CREAT, 0644, 0);

    // Initialize shared memory
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(SharedData) + sizeof(uint32_t) * processIds.size());
    
    // Map memory
    sharedData = static_cast<SharedData*>(mmap(0, sizeof(SharedData) + sizeof(uint32_t) * processIds.size(), PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0));

    // Initialize shared data
    sharedData->maxCriticalProcessID = maxCriticalProcessID;
    sharedData->registeredCriticalProcesses = 0;
    sharedData->registeredProcessesCount = 0;
    sharedData->processCount = processIds.size();

    for (uint32_t id : processIds)
        if (id < maxCriticalProcessID)
            sharedData->criticalProcessesCount.fetch_add(1);

    // Copy processIds vector to shared memory
    std::memcpy(sharedData->processIds, processIds.data(), processIds.size() * sizeof(uint32_t));

    // Set timeout handler
    struct sigaction sa;
    sa.sa_handler = &handle_timeout;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, NULL);
    
    // Initialize timer with the specified timeout
    struct itimerspec its;
    its.it_value.tv_sec = timeout;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;

    if (timer_create(CLOCK_REALTIME, NULL, &timerID) == -1) {
        perror("timer_create");
        exit(EXIT_FAILURE);
    }

    if (timer_settime(timerID, 0, &its, NULL) == -1) {
        perror("timer_settime");
        exit(EXIT_FAILURE);
    }
}

void SyncCommunication::initializeProcess()
{
    // Open semaphores
    startSem = sem_open(START_SEM_NAME, 0);

    // Connect to shared memory
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cout << "Failed to open shared memory" << std::endl;
        return;
    }

    // Map memory
    sharedData = static_cast<SharedData*>(mmap(0, sizeof(SharedData) + sizeof(uint32_t) * 3, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0));
    if (sharedData == MAP_FAILED) {
        std::cout << "Failed to map shared memory" << std::endl;
        return;
    }

    // Check if processIds is not null before accessing
    if (sharedData->processIds == nullptr || sharedData->processCount <= 0)
        return;
}

void SyncCommunication::registerProcess(uint32_t processId)
{ 
    if (sharedData->processIds == nullptr || sharedData->processCount <= 0) {
        std::cerr << "Error: sharedData->processIds is null or processCount is invalid." << std::endl;
        return;
    }
    
    int findProcessId = -1;
    for (size_t i = 0; i < sharedData->processCount; i++)
    {       
        std::cout << sharedData->processIds[i] << std::endl;
        if (sharedData->processIds[i] == processId)
            findProcessId = i;
    }

    if (findProcessId < 0) {
        return;
    }

    // Register the process if it's critical
    if (processId < sharedData->maxCriticalProcessID)
        sharedData->registeredCriticalProcesses.fetch_add(1);
    
    sharedData->registeredProcessesCount.fetch_add(1);

    // If all critical processes are registered, release them
    if (sharedData->registeredCriticalProcesses.load() == sharedData->criticalProcessesCount.load()) {
        for (int i = 0; i < sharedData->registeredProcessesCount.load(); ++i) {
            sem_post(startSem);
        }
    } else {
        // Wait for start signal
        sem_wait(startSem);
    }
}

// Static function to handle timeout
void SyncCommunication::handle_timeout(int signum)
{
    if (sharedData->registeredCriticalProcesses < sharedData->criticalProcessesCount) {
        //MainWindow::endProcesses();
    }
}

void SyncCommunication::close()
{
    sem_close(startSem);
    sem_unlink(START_SEM_NAME);

    // If this is the first process, release shared memory
    if (sharedData) {
        munmap(sharedData, sizeof(SharedData));
        shm_unlink(SHM_NAME);
    }
}

SyncCommunication::~SyncCommunication()
{
    close();
}