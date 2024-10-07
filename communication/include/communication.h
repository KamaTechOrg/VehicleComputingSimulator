#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <future>
#include <stdexcept>
#include "client_connection.h"
#include "error_code.h"
#include "scheduler.h"
#include "message.h"
#include "packet.h"

#define THRESHOLD_TO_BUSOFF 256  ///< Threshold for entering BusOff state
#define THRESHOLD_TO_PASSIVE 128 ///< Threshold for entering Passive state
#define BUSOFF_RECOVERY_TIME (128 * TICK_DURATION)      ///< Time required to recover from BusOff state
#define MAX_RETRANSMISSIONS 10 ///< Maximum allowed retransmissions for a packet
#define MAX_SIMULTANEOUS_MESSAGES 5 ///< Maximum number of messages allowed to be sent simultaneously

// Enumeration for different error states of the communication
enum class ErrorState {
    Active,  ///< The communication system is operating normally.
    Passive, ///< The communication system has issues and is losing priority during collisions.
    BusOff ///< The communication system is in BusOff state and cannot send or receive messages.
};

// Communication class to handle message transmission and reception
class Communication {
  private:
    ClientConnection client; ///< ClientConnection object for managing the connection
    std::unordered_map<std::string, Message> receivedMessages; ///< Map to store received messages
    void (*passData)(uint32_t, void *); ///< Callback function for passing data to the client
    uint32_t id; ///< Unique identifier for the communication instance
    static Communication *instance; ///< Static variable that holds an instance of the class

    // To limit simultaneous messages in asynchronous send
    static std::vector<std::future<void>> activeMessageFutures; ///< Vector to hold active message futures
    static std::mutex messageThreadsMutex; ///< Mutex to synchronize access to the active thread vector
    std::condition_variable messageThreadsCondition; ///< Condition variable to wait for available thread slots

    // For ensuring that only one send per tick occurs
    std::atomic<int> lastSendTick; ///< The last tick when a packet was sent
    std::mutex sendMutex;          ///< Mutex for synchronizing send operations

    // Error tracking variables
    std::mutex mtx;   ///< Mutex to protect access to error counts and state
    int TEC;          ///< Transmission Error Count
    int REC;          ///< Receive Error Count
    ErrorState state; ///< Current system state

    static std::thread busOffRecoveryThread; ///< Thread for handling BusOff recovery

    // RTO (Retransmission Timeout) handling
    Scheduler scheduler; ///< Scheduler for managing transmission timing

    // Packet reception and validation methods

    /**
     * @brief Receives a packet and processes it based on its type.
     * 
     * This method checks if the bus is in the BusOff state and exits if so. 
     * It then verifies the destination ID of the packet. 
     * If valid, it performs a CRC check for data integrity. 
     * If the CRC check fails, an error message is sent to the source.
     * 
     * Upon successful validation, the method determines the packet type:
     * - For data messages, it calls `handleDataPacket`.
     * - For error messages, it calls `handleErrorPacket`.
     * - For acknowledgment messages, it calls `handleAckPacket`.
     * 
     * @param packet The packet object to be received and processed.
     */
    void receivePacket(Packet &packet);

    /**
     * @brief Checks the destination ID of the packet.
     * 
     * This method determines whether the packet is intended for this communication node.
     * It returns true if the packet is either a broadcast or if its destination ID matches 
     * the ID of the current communication node.
     * 
     * @param packet The packet object containing destination information.
     * @return Returns true if the packet is a broadcast or if the destination ID matches the node's ID.
     */
    bool checkDestId(Packet &packet);

    /**
     * @brief Validates the CRC of the packet.
     * 
     * This method checks the integrity of the packet by validating its CRC (Cyclic Redundancy Check).
     * It calls the packet's own method to perform the CRC validation.
     * 
     * @param packet The packet object whose CRC is to be validated.
     * @return Returns true if the CRC is valid, false otherwise.
     */
    bool validCRC(Packet &packet);

    /**
     * @brief Handles incoming data packets.
     * 
     * This method processes data packets by adding them to the message queue.
     * After processing the packet, it sends an acknowledgment (ACK) back to the source.
     * 
     * @param packet The data packet to be handled.
     */
    void handleDataPacket(Packet &packet);

    /**
     * @brief Handles incoming error packets.
     * 
     * This method processes error packets based on their destination ID.
     * If the packet is intended for this node, it handles the transmission error.
     * Otherwise, it handles the reception error.
     * 
     * @param packet The error packet to be handled.
     */
    void handleErrorPacket(Packet &packet);

    /**
     * @brief Handles acknowledgment packets.
     * 
     * This method processes acknowledgment packets by retrieving the 
     * received message ID from the packet's payload. It then notifies 
     * the scheduler of the received acknowledgment and handles the 
     * successful transmission.
     * 
     * @param packet The acknowledgment packet to be handled.
     */
    void handleAckPacket(Packet &packet);

    /**
     * @brief Adds a received packet to the corresponding message.
     * 
     * This method checks if a message with the given ID already exists 
     * in the received messages. If it does, the packet is added to that 
     * message. If not, a new message is created. When the message is 
     * complete, the complete data is passed to the next stage of 
     * processing, and the message is removed from the received messages.
     * 
     * @param packet The packet to be added to the message.
     */
    void addPacketToMessage(Packet &packet);

    // BusOff recovery methods

    /**
     * @brief Initiates the recovery process from the BusOff state.
     * 
     * This method checks if the communication system is in the BusOff state 
     * and if the bus off recovery thread is not already running. If both conditions 
     * are met, it starts a new thread to handle the recovery process via the 
     * `busOffTimer` method.
     */
    void recoverFromBusOff();

    /**
     * @brief Handles the timer for recovering from the BusOff state.
     * 
     * This method puts the thread to sleep for a defined recovery time 
     * (BUSOFF_RECOVERY_TIME) and then resets the state of the communication system.
     */
    void busOffTimer();

    /**
     * @brief Cleans up the BusOff recovery thread.
     * 
     * This method checks if the BusOff recovery thread is joinable. 
     * If it is, the thread is joined to ensure proper cleanup and resource management.
     */
    static void cleanupBusOffRecoveryThread();

    // State checking and error handling methods

    /**
     * @brief Checks and updates the communication state based on error counters.
     * 
     * This method evaluates the Transmission Error Counter (TEC) and Receive Error Counter (REC) to determine 
     * the current state of the communication system. If TEC exceeds the threshold for BusOff, the state is 
     * set to BusOff. If TEC or REC exceeds the threshold for Passive, the state is set to Passive. 
     * Otherwise, the state is set to Active. If the state is BusOff, the recovery process is initiated.
     */
    void checkState();

    /**
     * @brief Handles the transmission error by updating the Transmission Error Counter (TEC).
     * 
     * This method increments the Transmission Error Counter (TEC) by a defined value (8) 
     * to reflect a transmission error. It then calls `checkState` to reassess the 
     * communication state based on the updated error counter.
     */
    void handleTransmitError();

    /**
     * @brief Handles acknowledgment errors by updating the Transmission Error Counter (TEC).
     * 
     * This method increments the Transmission Error Counter (TEC) by 1 to reflect 
     * an acknowledgment error. It then calls `checkState` to reassess the communication 
     * state based on the updated error counter.
     * 
     * @param packet The packet object associated with the acknowledgment error.
     */
    void handleACKError();

    /**
     * @brief Handles reception errors by updating the Reception Error Counter (REC).
     * 
     * This method increments the Reception Error Counter (REC) by 1 to reflect 
     * a reception error. It then calls `checkState` to reassess the communication 
     * state based on the updated error counter.
     */
    void handleReceiveError();

    /**
     * @brief Handles successful transmissions by updating the Transmit Error Counter (TEC).
     * 
     * This method decrements the Transmit Error Counter (TEC) by 1 to reflect 
     * a successful transmission, as long as the counter is greater than zero. 
     * It then calls `checkState` to reassess the communication state based on 
     * the updated error counter.
     */
    void handleSuccessfulTransmission();

    /**
     * @brief Resets the communication state by clearing error counters and setting state to active.
     * 
     * This method resets the Transmit Error Counter (TEC) and Receive Error Counter (REC) 
     * to zero, and sets the communication state to `Active`. This function ensures that the 
     * communication system starts fresh after experiencing issues, allowing normal operation 
     * to resume.
     */
    void resetState();

    //Management of packet transmission according to clock ticks.

    /**
     * @brief Ensures that only one message is sent per clock tick.
     * 
     * This method checks the current clock tick and compares it with the last tick 
     * when a message was sent. If the current tick is different, it updates the 
     * last sent tick. If it is the same, the function waits for the next tick 
     * before proceeding.
     * 
     * The method uses a mutex to ensure thread safety during the tick check and 
     * update operations.
     */
    void ensureSingleSendPerTick();

    /**
     * @brief Sends a packet with a retransmission timeout (RTO).
     * 
     * This method attempts to send a packet, checking the system state and the 
     * number of retransmissions. If the system is in the BusOff state or the 
     * maximum retransmissions have been exceeded, it stops sending the packet.
     * 
     * It ensures that only one packet is sent per tick and sets the packet's 
     * passive state based on the current system state. It waits for an acknowledgment 
     * (ACK) and handles retransmissions if needed.
     * 
     * @param packet The packet object to be sent.
     * @param retransmissions The number of retransmissions attempted (default is 0).
     * @return True if the packet was sent successfully, false otherwise.
     */
    bool sendPacketWithRTO(Packet &packet, int retransmissions = 0);

    /**
     * @brief Sends a packet to the designated recipient.
     * 
     * This method checks if the system is in the BusOff state. If it is, the function 
     * immediately returns false. It ensures that only one packet is sent per tick 
     * and sets the packet's passive state based on the current system state.
     * 
     * The method attempts to send the packet using the client and returns true if 
     * the transmission is successful.
     * 
     * @param packet The packet object to be sent.
     * @return True if the packet was sent successfully, false otherwise.
     */
    bool sendPacket(Packet &packet);

    /**
     * @brief Waits for all active message threads to complete.
     * 
     * This function manages the synchronization of active message threads in a thread-safe manner. 
     * It locks the mutex to ensure thread safety while iterating through the futures of active message threads. 
     * Each valid future is waited upon to ensure that all active messages have been processed before clearing the vector.
     */
    static void waitForActiveMessages();

    /**
     * @brief Handles signals for the communication instance.
     * 
     * This function is called when a signal is received. It performs cleanup operations,
     * including closing the client connection and waiting for active message threads to complete.
     * It exits the program with the provided signal number.
     *
     * @param signum The signal number that triggered this handler.
     */
    static void signalHandler(int signum);

    /**
     * @brief Sets the ID for the communication instance.
     * 
     * This function updates the ID used for communication purposes.
     *
     * @param newId The new ID to be set for the instance.
     */
    void setId(uint32_t newId);

    /**
     * @brief Sets the callback function for passing data.
     * 
     * This function allows setting a custom callback function that will be called to pass data.
     *
     * @param callback A pointer to the function that will handle data passing.
     */
    void setPassDataCallback(void (*callback)(uint32_t, void *));

  public:
    //Cconstructor and destructor
    /**
     * @brief Constructs a Communication object.
     * 
     * This constructor initializes the communication instance with the given ID and 
     * sets the callback for passing data. It also sets a signal handler for SIGINT 
     * and initializes the error counters and state.
     *
     * @param id The unique identifier for the communication instance.
     * @param passDataCallback The callback function to pass data.
     */
    Communication(uint32_t id, void (*passDataCallback)(uint32_t, void *));
    /**
     * @brief Destroys the Communication object.
     * 
     * This destructor performs cleanup operations, including waiting for active 
     * message threads and cleaning up any recovery threads.
     */
    ~Communication();

    //connection management

    /**
     * @brief Starts the connection to the server.
     * 
     * This function attempts to establish a connection with the server using the 
     * communication instance's ID.
     *
     * @return ErrorCode indicating the result of the operation.
     */
    ErrorCode startConnection();

    //Sending messages

    /**
     * @brief Sends a message synchronously.
     * 
     * This function sends a message to a specified destination ID with the provided 
     * data and size. It validates input parameters before sending and returns an 
     * error code based on the result of the sending operation.
     *
     * @param data Pointer to the data to send.
     * @param dataSize Size of the data in bytes.
     * @param destID The destination ID for the message.
     * @param srcID The source ID of the message.
     * @param messageType The type of message being sent (default is DATA_MESSAGE).
     * @return ErrorCode indicating the result of the operation.
     */
    ErrorCode sendMessage(void *data, size_t dataSize, uint32_t destID,
                          uint32_t srcID,
                          MessageType messageType = MessageType::DATA_MESSAGE);

    /**
     * @brief Sends a message asynchronously.
     * 
     * This function sends a message to a specified destination ID asynchronously, 
     * allowing the calling thread to continue without blocking. It accepts a callback 
     * function that will be called with the result of the sending operation.
     *
     * @param data Pointer to the data to send.
     * @param dataSize Size of the data in bytes.
     * @param destID The destination ID for the message.
     * @param srcID The source ID of the message.
     * @param sendCallback The callback function to call with the result of the send operation.
     * @param messageType The type of message being sent.
     */
    void sendMessageAsync(void *data, size_t dataSize, uint32_t destID,
                          uint32_t srcID,
                          std::function<void(ErrorCode)> sendCallback,
                          MessageType messageType = MessageType::DATA_MESSAGE);

    /**
     * @brief Retrieves the current state of the communication system.
     * 
     * This method returns the current communication state, which may be one of the following:
     * - Active
     * - Passive
     * - BusOff
     * 
     * The function locks the mutex to ensure thread safety while accessing the state.
     * 
     * @return ErrorState The current communication state.
     */
    ErrorState getState();
};

#endif // __COMMUNICATION_H__
