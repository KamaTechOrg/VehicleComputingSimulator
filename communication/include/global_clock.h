#ifndef __GLOBAL_CLOCK_H__
#define __GLOBAL_CLOCK_H__
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <csignal>
#include <pthread.h>
#include <limits> 

#define SHM_NAME "/clock__shm"
#define TICK_DURATION std::chrono::milliseconds(50) // Define the tick duration
#define MAX_TICK std::numeric_limits<int>::max() // Define the maximum value for the tick

/**
 * @struct SharedClock
 * @brief A structure representing the shared state of the global clock.
 *
 * This structure is stored in shared memory, allowing multiple processes to access and modify
 * the clock state. It contains:
 * - `current_tick`: The current value of the clock tick.
 * - `is_running`: A boolean flag indicating whether the clock is running.
 * - `tick_mutex`: A mutex used to protect access to the tick count.
 * - `tick_cond`: A condition variable used to notify waiting processes when a tick occurs.
 */
struct SharedClock {
    std::atomic<int> current_tick; ///< The current tick value.
    std::atomic<bool> is_running;   ///< Whether the clock is currently running.
    pthread_mutex_t tick_mutex;     ///< Mutex to synchronize access to the tick count.
    pthread_cond_t tick_cond;       ///< Condition variable to notify waiting threads on tick change.
};
/**
 * @class GlobalClock
 * @brief A class that manages a global clock accessible across multiple processes using shared memory.
 *
 * The `GlobalClock` class is responsible for maintaining a synchronized global clock that increments
 * ticks at regular intervals. This clock can be shared across multiple processes using shared memory,
 * and synchronization is achieved using mutexes and condition variables to ensure that processes can
 * wait for the next tick and query the current tick in a thread-safe manner.
 *
 * The clock can be started and stopped, and it ensures proper cleanup of shared resources when the
 * process is terminated.
 */
class GlobalClock {
  public:
    /**
     * @brief Starts the global clock.
     *
     * Initializes shared memory if not already initialized, sets the clock to running,
     * and begins ticking at regular intervals in a separate thread. Only one process can start the clock.
     */
    static void startClock();
    /**
     * @brief Waits for the next tick.
     *
     * Blocks the calling thread until the next clock tick occurs, allowing synchronization
     * with the global clock. Uses condition variables to wait for the clock tick to change.
     */
    static void waitForNextTick();
    /**
     * @brief Retrieves the current tick value.
     *
     * @return The current value of the clock tick, as an integer.
     *
     * Ensures the shared memory is initialized, and returns the current tick count stored in
     * the shared memory.
     */
    static int getCurrentTick();
    /**
     * @brief Stops the global clock.
     *
     * Sets the `is_running` flag in the shared memory to `false`, stopping the clock and
     * ensuring that the clock thread is properly joined before exiting.
     */
    static void stopClock();

  private:
    /**
     * @brief Initializes shared memory for the global clock if not already initialized for this process.
     *
     * Allocates shared memory and initializes the `SharedClock` structure if necessary, which contains the
     * current tick, running state, mutex, and condition variable.
     */
    static void initializeSharedMemory();
    /**
     * @brief .Initializes the mutex and condition variables for shared clock synchronization.
     *
     * This function is only called by the first process that starts the clock.
     * It sets the mutex and condition variable to be shared between processes
     * using `PTHREAD_PROCESS_SHARED`.
     */
    static void initializeClockSynchronization();
    /**
     * @brief Releases process-specific resources.
     *
     * Unmaps the shared memory and closes the file descriptor associated with it.
     * This method ensures that the process-specific resources are properly cleaned up.
     */
    static void releaseProcessResources();
    /**
     * @brief Releases system-wide shared resources.
     *
     * Unlinks the shared memory, effectively deleting it from the system, and closes the file
     * descriptor. This should be called when no other process requires access to the clock.
     */
    static void releaseSharedSystemResources();
    /**
     * @brief Registers a cleanup routine.
     *
     * Registers a cleanup routine using `atexit()` to ensure that when the process exits, it
     * properly releases its resources, whether those are process-specific or system-wide.
     */
    static void registerCleanup();
    /**
     * @brief The main function to run the clock.
     *
     * Increments the tick count in regular intervals (defined by `TICK_DURATION`) while the
     * clock is running. Broadcasts the condition variable on each tick to wake up waiting processes.
     */
    static void runClock();
    // Shared memory for the clock state, accessible across processes.
    static SharedClock *shared_clock;
    // File descriptor for the shared memory.
    static int shared_memory_fd;
    // Thread to manage the clock in the background.
    static std::thread clock_thread;
};
#endif // __GLOBAL_CLOCK_H__