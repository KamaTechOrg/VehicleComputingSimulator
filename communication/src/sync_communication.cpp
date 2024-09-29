#include "../include/sync_communication.h"

std::atomic<int> SyncCommunication::registeredCriticalProcesses;  
std::atomic<int> SyncCommunication::criticalProcessesCount;
int SyncCommunication::maxCriticalProcessID;
std::atomic<int> SyncCommunication::registeredProcessesCount;
std::vector<uint32_t> SyncCommunication::processIDs;
timer_t SyncCommunication::timerID;

SyncCommunication::SyncCommunication()
{
}

ErrorCode SyncCommunication::initializeManager(const std::vector<uint32_t>& processIds, int maxCriticalProcessId)
{
    // Initialize data
    maxCriticalProcessID = maxCriticalProcessId;
    registeredCriticalProcesses = 0;
    registeredProcessesCount = 0;
    processIDs = processIds;

    for (uint32_t id : processIds)
        if (id < maxCriticalProcessID)
            criticalProcessesCount.fetch_add(1);

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

    auto timerCreateRes = timer_create(CLOCK_REALTIME, NULL, &timerID);
    if (timerCreateRes == -1)
        return ErrorCode::TIMER_CREATE_FAILED;

    auto timerSettimeRes = timer_settime(timerID, 0, &its, NULL);
    if (timerSettimeRes == -1) 
      return ErrorCode::TIMER_SETTIME_FAILED;
      
    return ErrorCode::SUCCESS;
}

ErrorCode SyncCommunication::registerProcess(uint32_t processId)
{ 
    auto findProcessId = std::find(processIDs.begin(), processIDs.end(), processId);

    if (findProcessId == processIDs.end())
        return ErrorCode::INVALID_ID;

    // Register the process if it's critical
    if (processId < maxCriticalProcessID)
        registeredCriticalProcesses.fetch_add(1);
    
    registeredProcessesCount.fetch_add(1);
    std::cout << "registeredProcessesCount "<<registeredProcessesCount.load()<< std::endl;
    std::cout << "registeredCriticalProcesses "<<registeredCriticalProcesses.load()<< std::endl;
    std::cout << "criticalProcessesCount "<<criticalProcessesCount.load()<< std::endl;

    // If all critical processes are registered, release them
    if (registeredCriticalProcesses.load() != criticalProcessesCount.load()) 
        return ErrorCode::NOT_SYNCHRONIZED;
  
    return ErrorCode::SUCCESS;
}

// Static function to handle timeout
void SyncCommunication::handle_timeout(int signum)
{
    if (registeredCriticalProcesses < criticalProcessesCount) {
        std::cout<<"timeout"<<std::endl;
        //MainWindow::endProcesses();
    }
}

SyncCommunication::~SyncCommunication()
{
}