#include "../include/global_clock.h"

SharedClock *GlobalClock::shared_clock = nullptr;
int GlobalClock::shared_memory_fd = -1;
std::thread GlobalClock::clock_thread;

void GlobalClock::startClock()
{
    initializeSharedMemory(); // Initialize shared memory if not done yet
    if (shared_clock->is_running.load()) { // Check if the clock is already running
        RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Clock is already running.");
        return; // Exit if clock is running
    }
    initializeClockSynchronization(); // Initialize the mutex and condition variables for shared clock synchronization.
    shared_clock->is_running.store(true); // Set the clock state to running
    RealSocket::log.logMessage(logger::LogLevel::INFO, "Clock started.");
    clock_thread = std::thread(runClock); // Run the clock in a new thread
}

int GlobalClock::getCurrentTick()
{
    initializeSharedMemory(); // Ensure shared memory is initialized before accessing the tick
    int currentTick = shared_clock->current_tick.load();
    return currentTick; // Return the current tick value
}

void GlobalClock::waitForNextTick()
{
    initializeSharedMemory(); // Ensure shared memory is initialized
    pthread_mutex_lock(&shared_clock->tick_mutex); // Lock the mutex to access the tick safely
    pthread_cond_wait(&shared_clock->tick_cond, &shared_clock->tick_mutex); // Wait for tick condition signal
    pthread_mutex_unlock(&shared_clock->tick_mutex); // Unlock the mutex after tick change
}

void GlobalClock::stopClock()
{
    if (shared_clock->is_running.load()) {
        RealSocket::log.logMessage(logger::LogLevel::INFO, "Stopping the clock...");
        shared_clock->is_running.store(false); // Stop the clock
        if (clock_thread.joinable()) {
            clock_thread.join(); // Wait for the clock thread to finish
        }
        RealSocket::log.logMessage(logger::LogLevel::INFO, "Clock stopped.");
    }
}

void GlobalClock::initializeSharedMemory()
{
    if (shared_clock == nullptr) { // Check if shared memory is already initialized
        shared_memory_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666); // Open shared memory file descriptor
        if (shared_memory_fd == -1) {
            RealSocket::log.logMessage(logger::LogLevel::ERROR, "Error accessing shared memory for the clock.");
            exit(1); // Exit if there's an error accessing shared memory
        }
        ftruncate(shared_memory_fd, sizeof(SharedClock)); // Set size of shared memory
        shared_clock = static_cast<SharedClock *>(mmap(0, sizeof(SharedClock), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_fd, 0)); // Map shared memory
        if (shared_clock == MAP_FAILED) {
            RealSocket::log.logMessage(logger::LogLevel::ERROR, "Error mapping shared memory for the clock.");
            exit(1); // Exit if there's an error mapping shared memory
        }
        RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Shared memory for the clock initialized.");
        registerCleanup(); // Register cleanup function to be called at exit
    }
}

void GlobalClock::initializeClockSynchronization()
{
    // Initialize mutex and condition variables for shared clock synchronization
    pthread_mutexattr_t mutex_attr;
    pthread_condattr_t cond_attr;

    // Initialize the mutex attribute and set it to be shared between processes
    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED); // Set mutex to be shared between processes
    pthread_mutex_init(&shared_clock->tick_mutex, &mutex_attr); // Initialize mutex in shared memory

    // Initialize the condition attribute and set it to be shared between processes
    pthread_condattr_init(&cond_attr);
    pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED); // Set condition variable to be shared between processes
    pthread_cond_init(&shared_clock->tick_cond, &cond_attr); // Initialize condition variable in shared memory
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Clock synchronization initialized.");
}

void GlobalClock::releaseProcessResources()
{
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Releasing process-specific resources for the clock...");
    if (shared_clock != nullptr) {
        munmap(shared_clock, sizeof(SharedClock)); // Unmap the shared memory from the process address space
        close(shared_memory_fd); // Close the file descriptor for shared memory
        shared_clock = nullptr;  // Reset pointer to shared clock
        shared_memory_fd = -1;   // Reset shared memory file descriptor
        RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Process-specific resources for the clock released.");
    }
}

void GlobalClock::releaseSharedSystemResources()
{
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Releasing system-wide shared resources for the clock...");
    if (shared_memory_fd != -1) {
        shm_unlink(SHM_NAME); // Unlink shared memory from the system (delete it)
        close(shared_memory_fd); // Close the file descriptor for shared memory
        shared_memory_fd = -1;   // Reset shared memory file descriptor
        RealSocket::log.logMessage(logger::LogLevel::DEBUG, "System-wide shared resources for the clock released.");
    }
}

void GlobalClock::registerCleanup()
{
    atexit([]() { // Register a cleanup function to be called when the process exits
        if (shared_clock->is_running.load()) {
            GlobalClock::releaseProcessResources(); // Release process-specific resources if the clock is running
        }
        else {
            GlobalClock::releaseSharedSystemResources(); // Release system-wide resources if the clock is not running
        }
    });
}

void GlobalClock::runClock()
{
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Clock thread started.");
    while (shared_clock->is_running.load()) {
        std::this_thread::sleep_for(TICK_DURATION); // Sleep for tick duration
        pthread_mutex_lock(&shared_clock->tick_mutex); // Lock the mutex to safely increment the tick
        // Increment the tick and check for overflow
        if (shared_clock->current_tick.load() >= MAX_TICK) {
            shared_clock->current_tick.store(0); // Reset to 0 if max is reached
            RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Tick overflow, resetting to 0.");
        }
        else {
            shared_clock->current_tick++; // Increment the tick
        }
        pthread_cond_broadcast(&shared_clock->tick_cond); // Notify all waiting processes that the tick has changed
        pthread_mutex_unlock(&shared_clock->tick_mutex); // Unlock the mutex after tick change
    }
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Clock thread stopped.");
}
