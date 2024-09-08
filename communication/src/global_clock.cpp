#include "global_clock.h"

// Initialize static members
std::mutex GlobalClock::s_cvMutex;
std::condition_variable GlobalClock::s_cv;
std::thread GlobalClock::s_clockThread;
std::atomic<bool> GlobalClock::s_running(false);
std::atomic<int> GlobalClock::s_simulationTime(0);

void GlobalClock::start() {
  if (s_running.load())
    return;                         // Already running, do nothing
  s_running.store(true);            // Set the running flag to true
  s_clockThread = std::thread(run); // Start the clock thread
}

void GlobalClock::stop() {
  s_running.store(false); // Set the running flag to false
  if (s_clockThread.joinable()) {
    s_cv.notify_all();    // Notify all waiting threads to ensure they can exit
    s_clockThread.join(); // Wait for the clock thread to finish
  }
}

void GlobalClock::waitForNextSimulationCycle() {
  std::unique_lock<std::mutex> lock(
      s_cvMutex); // Lock the mutex to access shared data
  int currentTime = s_simulationTime.load(); // Save the current time
  s_cv.wait(lock, [currentTime] {
    return s_simulationTime.load() > currentTime;
  }); // Wait until simulationTime is updated
}

void GlobalClock::signalNextSimulationCycle() {
  ++s_simulationTime; // Increment the simulation time
  s_cv.notify_all();  // Notify all waiting threads that the cycle has started
}

int GlobalClock::getCurrentSimulationTime() {
  return s_simulationTime.load(); // Return the current value of simulationTime
}

void GlobalClock::run() {
  while (s_running.load()) { // Continue running while the clock is active
    std::this_thread::sleep_for(
        CLOCK_CYCLE_DURATION);   // Simulate the duration of a clock cycle
    signalNextSimulationCycle(); // Notify that a new cycle has started
  }
}
