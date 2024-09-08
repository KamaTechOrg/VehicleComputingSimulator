#ifndef __GLOBAL_CLOCK_H__
#define __GLOBAL_CLOCK_H__

#define CLOCK_CYCLE_DURATION std::chrono::milliseconds(500)

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

/**
 * @class GlobalClock
 * @brief Manages a global simulation clock for timing simulation cycles.
 *
 * The GlobalClock class provides functionality to start, stop, and wait for
 * simulation cycles. It runs on a separate thread and notifies other threads
 * when each cycle begins. It also provides access to the current simulation
 * time.
 */

class GlobalClock {
public:
  // Starts the global clock
  static void start();

  // Stops the global clock
  static void stop();

  // Waits for the next simulation cycle to start
  static void waitForNextSimulationCycle();

  // Returns the current simulation time
  static int getCurrentSimulationTime();

private:
  // The function that runs in the clock thread
  static void run();

  // Signals the start of the next simulation cycle
  static void signalNextSimulationCycle();

  // Mutex for synchronizing access to condition variable and shared data
  static std::mutex s_cvMutex;

  // Condition variable for waiting and notifying threads
  static std::condition_variable s_cv;

  // Thread for running the clock
  static std::thread s_clockThread;

  // Atomic flag indicating whether the clock is running
  static std::atomic<bool> s_running;

  // Atomic integer to track the number of simulation cycles
  static std::atomic<int> s_simulationTime;
};

#endif // __GLOBAL_CLOCK_H__
