#include "../include/scheduler.h"

Scheduler::Scheduler()
{
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Scheduler created.");
}

Scheduler::~Scheduler()
{
    stopAllTimers();
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Scheduler destroyed and all timers stopped.");
}

void Scheduler::stopAllTimers()
{
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Stopping all timers.");
    for (auto &future : futures)
    {
        if (future.valid())
            future.wait(); // Wait for all threads to finish
    }
}

void Scheduler::startRetransmissionTimer(int packetID, Callback callback, std::shared_ptr<std::promise<bool>> ackPromise)
{
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Starting retransmission timer for packet of message ID: "
                                                        + std::to_string(packetID));

    // Promise to manage the lifecycle of the thread itself
    std::promise<void> threadCompletionPromise;
    
    // Future object to track the thread's completion
    std::future<void> future = threadCompletionPromise.get_future();
    
    // Store the future to ensure we can wait for the thread to finish later
    futures.push_back(std::move(future));

    // Start a new thread for handling retransmission and ACK wait
    std::thread([this, packetID, callback, threadCompletionPromise = std::move(threadCompletionPromise), ackPromise]() mutable
                {
                    int retryCount = 0;
                    {
                        std::unique_lock<std::mutex> lock(mutex);

                        // Wait for an ACK or timeout
                        if (cv.wait_for(lock, MAX_ACK_TIMEOUT, [this, packetID]()
                                        { return ackReceived[packetID]; }))
                        {
                            // ACK received within the timeout period
                            RealSocket::log.logMessage(logger::LogLevel::DEBUG, "ACK received for packet of message ID: "
                                                                                + std::to_string(packetID));

                            clearPacketData(packetID); // Clear packet data
                            
                            // Set both promises to indicate success and thread completion
                            threadCompletionPromise.set_value(); 
                            ackPromise->set_value(true); // ACK was received, set to true
                            return; // Exit the thread
                        }
                        else
                        {
                            // Timeout occurred, retransmit the packet
                            retryCounts[packetID]++;
                            retryCount = retryCounts[packetID];
                            RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Retransmitting packet of message ID: "
                                                                                 + std::to_string(packetID)
                                                                                 + ", retry count: " + std::to_string(retryCount));
                        }
                    }
                    
                    // Call the callback function with the updated retry count
                    callback(retryCount);
                    
                    // Set the promise to indicate the thread has finished
                    threadCompletionPromise.set_value(); 
                })
        .detach(); // Detach the thread to allow it to run independently
}

void Scheduler::receiveACK(int packetID)
{
    std::unique_lock<std::mutex> lock(mutex);
    ackReceived[packetID] = true;
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "ACK received for packet of message ID: "
                                                        + std::to_string(packetID)
                                                        + ". Notifying all waiting threads.");
    cv.notify_all(); // Notify all waiting threads
}

void Scheduler::clearPacketData(int packetID)
{
    ackReceived.erase(packetID);
    retryCounts.erase(packetID);
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Cleared data for packet of message ID: "
                                                        + std::to_string(packetID));
}
