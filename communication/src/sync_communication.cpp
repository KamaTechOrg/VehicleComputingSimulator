#include "../include/sync_communication.h"

std::atomic<int> SyncCommunication::registeredCriticalProcesses;  
std::atomic<int> SyncCommunication::criticalProcessesCount;
int SyncCommunication::maxCriticalProcessID;
std::atomic<int> SyncCommunication::registeredProcessesCount;
std::vector<uint32_t> SyncCommunication::processIDs;
timer_t SyncCommunication::timerID;
bool SyncCommunication::critical_ready;
std::mutex SyncCommunication::mutexCV;
std::condition_variable SyncCommunication::cv;

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
    critical_ready = false;

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
    if (timerCreateRes == -1){
        RealSocket::log.logMessage(logger::LogLevel::ERROR, "Failed to create timer: " + std::string(strerror(errno)));
        return ErrorCode::TIMER_CREATE_FAILED;
    }

    auto timerSettimeRes = timer_settime(timerID, 0, &its, NULL);
    if (timerSettimeRes == -1) {
        RealSocket::log.logMessage(logger::LogLevel::ERROR, "Failed to set timer: " + std::string(strerror(errno)));
        return ErrorCode::TIMER_SETTIME_FAILED;
    }
    RealSocket::log.logMessage(logger::LogLevel::INFO, "Manager initialized successfully, Critical processes count initialized to: " + std::to_string(criticalProcessesCount));

    return ErrorCode::SUCCESS;
}

ErrorCode SyncCommunication::registerProcess(uint32_t processId)
{ 
    auto findProcessId = std::find(processIDs.begin(), processIDs.end(), processId);

    if (findProcessId == processIDs.end()){
        RealSocket::log.logMessage(logger::LogLevel::ERROR, "Invalid process ID: " + std::to_string(processId));
        return ErrorCode::INVALID_ID;
    }

    // Register the process if it's critical
    if (processId < maxCriticalProcessID)
        registeredCriticalProcesses.fetch_add(1);
    
    registeredProcessesCount.fetch_add(1);

    if (registeredCriticalProcesses.load() != criticalProcessesCount.load()) {
        RealSocket::log.logMessage(logger::LogLevel::ERROR, "Critical processes not synchronized. Registered: " + std::to_string(registeredCriticalProcesses) + ", Expected: " + std::to_string(criticalProcessesCount));
        return ErrorCode::NOT_SYNCHRONIZED;
    }
    
    {
        std::lock_guard<std::mutex> lock(mutexCV);
        critical_ready = true;
    }
    cv.notify_all();
    RealSocket::log.logMessage(logger::LogLevel::INFO, "Critical processes synchronized and ready.");

    return ErrorCode::SUCCESS;
}

// Static function to handle timeout
void SyncCommunication::handle_timeout(int signum)
{
    if (registeredCriticalProcesses < criticalProcessesCount) {
        RealSocket::log.logMessage(logger::LogLevel::ERROR, "Timeout occurred. Critical processes not fully registered.");
        //MainWindow::endProcesses();
    }
}

SyncCommunication::~SyncCommunication()
{
}