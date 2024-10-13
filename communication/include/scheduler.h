#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <unordered_map>
#include <vector>
#include "global_clock.h"
#include "../sockets/real_socket.h"
// must be set
#define MAX_ACK_TIMEOUT \
    (50 * TICK_DURATION) ///< Maximum time to wait for ACK

/**
 * @class Scheduler
 * @brief Manages retransmission timers and tracks acknowledgments for packets.
 *
 * This class provides functionality to manage retransmission timers for
 * packets, handle acknowledgments, and clean up packet data once
 * retransmissions are complete.
 */
class Scheduler
{
public:
    using Callback = std::function<void(int)>;

    Scheduler();
    ~Scheduler();

    /**
     * @brief Stops all active timers and waits for their completion.
     *
     * This method ensures that all active timers (threads) complete their
     * execution before the program exits.
     */
    void stopAllTimers();

    /**
     * @brief Starts a retransmission timer for a given packet ID.
     * 
     * The function initiates a timer to wait for an ACK (Acknowledgment) for the specified packet.
     * If the ACK is received within the MAX_ACK_TIMEOUT, it clears the packet data and sets the result 
     * of the ackPromise to `true`, indicating success. If the timeout occurs and no ACK is received,
     * it triggers the provided callback function to retransmit the packet and increments the retry count.
     * 
     * @param packetID The unique ID of the packet being transmitted.
     * @param callback The callback function to call when retransmitting the packet after a timeout.
     * @param ackPromise A shared promise that communicates whether the packet transmission was successful 
     *                   (i.e., ACK was received).
     */
    void startRetransmissionTimer(int packetID, Callback callback, std::shared_ptr<std::promise<bool>> ackPromise);

    /**
     * @brief Receives an acknowledgment for a packet.
     *
     * @param packetID The ID of the packet that has been acknowledged.
     */
    void receiveACK(int packetID);

    /**
     * @brief Clears the data associated with a packet.
     *
     * @param packetID The ID of the packet whose data is to be cleared.
     */
    void clearPacketData(int packetID);

private:
    std::unordered_map<int, bool>
        ackReceived; ///< Map to track received acknowledgments
    std::unordered_map<int, int>
        retryCounts;            ///< Map to track retry counts for packets
    std::mutex mutex;           ///< Mutex for thread safety
    std::condition_variable cv; ///< Condition variable for synchronization
    std::vector<std::future<void>>
        futures; ///< Vector to store futures of active threads
};

#endif // __SCHEDULER_H__
