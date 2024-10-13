#include "../include/communication.h"

// Initialize static member variables for the Communication class
Communication *Communication::instance = nullptr;
std::thread Communication::busOffRecoveryThread;
std::mutex Communication::messageThreadsMutex;
std::vector<std::future<void>> Communication::activeMessageFutures;

// Constructor
Communication::Communication(uint32_t id, void (*passDataCallback)(uint32_t, void *))
    : TEC(0), REC(0), state(ErrorState::Active),
      client(std::bind(&Communication::receivePacket, this,
                       std::placeholders::_1)),
      lastSendTick(GlobalClock::getCurrentTick())
{
    setId(id);
    setPassDataCallback(passDataCallback);
    instance = this;
    setSignaleHandler();
}

// Sends the client to connect to server
ErrorCode Communication::startConnection()
{
    ErrorCode isConnected = client.connectToServer(processID);
    //Increases the shared memory and blocks the process - if not all are connected
    return isConnected;
}

// Send message synchronously
ErrorCode Communication::sendMessage(
    void *data, size_t dataSize, uint32_t destID, uint32_t srcID,
    MessageType messageType /*= MessageType::DATA_MESSAGE*/)
{
    if (dataSize == 0)
        return ErrorCode::INVALID_DATA_SIZE; // Check for valid data size
    if (data == nullptr)
        return ErrorCode::INVALID_DATA; // Check for valid data pointer
    if (destID == 0 || srcID == 0)
        return ErrorCode::INVALID_ID; // Check for valid IDs
    if (!client.isConnected())
        return ErrorCode::CONNECTION_FAILED; // Check for connection status

    Message msg(srcID, destID, data, dataSize,messageType); // Create a new message

    //Sending the message to logger
    RealSocket::log.logMessage(logger::LogLevel::INFO, "Start sending "
                                                          + Message::getMessageTypeString(messageType)
                                                          + " message with ID: "
                                                          + std::to_string(msg.getMessageID())
                                                          + " Complete message:"
                                                          +Packet::pointerToHex(data ,dataSize));
   
    for (auto &packet : msg.getPackets()) {
#ifdef ESP32
          delay(80);
#endif
        bool success =
            (messageType == MessageType::DATA_MESSAGE)
                ? sendPacketWithRTO(packet) // Send with retransmission timeout
                : sendPacket(packet);       // Send packet directly
        if (!success) {
            if( messageType == MessageType::DATA_MESSAGE)
                RealSocket::log.logMessage(logger::LogLevel::ERROR, "Failed sending "
                                                                    + Message::getMessageTypeString(messageType)
                                                                    + " message with ID: "
                                                                    + std::to_string(msg.getMessageID()) + ".");
            else
                RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Failed sending messageID: " + std::to_string(msg.getMessageID()));
            return ErrorCode::SEND_FAILED; // Return failure if sending fails
        }
    }
    RealSocket::log.logMessage(logger::LogLevel::INFO, "Finished sending "
                                                          + Message::getMessageTypeString(messageType)
                                                          + " message with ID: "
                                                          + std::to_string(msg.getMessageID())
                                                          + " successfully.");

    return ErrorCode::SUCCESS; // Return success if all packets are sent
}       

// Send message asynchronously
void Communication::sendMessageAsync(
    void *data, size_t dataSize, uint32_t destID, uint32_t srcID,
    std::function<void(ErrorCode)> sendCallback, MessageType messageType)
{
    std::unique_lock<std::mutex> lock(messageThreadsMutex);
    // Wait until the number of active threads is below the maximum limit
    while (activeMessageFutures.size() >= MAX_SIMULTANEOUS_MESSAGES) {
        RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Maximum simultaneous messages reached. Waiting for available thread...");
        messageThreadsCondition.wait(lock); // Wait for a thread to complete
    }
    // Print the number of active threads (i.e. messages being sent)
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, 
        "Number of messages being sent simultaneously: " 
        + std::to_string(activeMessageFutures.size() + 1)); // +1 to include the new thread

    // Create a new promise and future
    std::promise<void> promise;
    std::future<void> future = promise.get_future();
    activeMessageFutures.push_back(std::move(future)); // Store future

    // Create a new thread to handle message sending
    std::thread([this, data, dataSize, destID, srcID, messageType, sendCallback,
                 promise = std::move(promise)]() mutable {
        ErrorCode result = this->sendMessage(data, dataSize, destID, srcID,
                                             messageType); // Send the message
        sendCallback(result); // Call the callback after the message is sent

        // Notify that the thread work is done
        promise.set_value(); // Set the promise value

        // Lock the mutex to modify the active thread list
        std::unique_lock<std::mutex> lock(this->messageThreadsMutex);

        // Remove the promise from the vector of active futures
        auto it = std::remove_if(
            activeMessageFutures.begin(), activeMessageFutures.end(),
            [](std::future<void> &future) {
                return future.wait_for(std::chrono::seconds(0)) ==
                       std::future_status::
                           ready; // Check if the future is ready
            });
        activeMessageFutures.erase(
            it, activeMessageFutures.end()); // Remove invalid futures
        // Notify one waiting thread that space is available
        this->messageThreadsCondition.notify_one();
    }).detach(); // Detach the thread
}

void Communication::receivePacket(const Packet &packet)
{
    // Check if the bus is in BusOff state
    // If the system is in BusOff state, exit the function and do not process the packet
    if (getState() == ErrorState::BusOff) {
        RealSocket::log.logMessage(logger::LogLevel::ERROR, "Cannot receive packet "
                                                            + std::to_string(packet.getPSN())
                                                            + ",of message ID: "
                                                            + std::to_string(packet.getId())
                                                            + ", system in BusOff state.");
        return;
    }

    // Check if the destination ID of the packet matches the expected destination
    if (checkDestId(packet)) {
        // Check CRC validity
        // Validate the packet's CRC for data integrity
        if (!validCRC(packet)) {
          // Log the CRC error with packet ID and other information
            RealSocket::log.logMessage(logger::LogLevel::ERROR, "Invalid CRC detected for packet "
                                                                + std::to_string(packet.getPSN())
                                                                + " of message ID: "
                                                                + std::to_string(packet.getId()) + ".");
            // Retrieve the packet ID
            uint32_t packetId = packet.getId();
            // Send an error message back to the source with the invalid packet ID
            sendMessage(&packetId, sizeof(packetId), packet.getSrcId(),
                        packet.getDestId(), MessageType::ERROR_MESSAGE);

            RealSocket::log.logMessage(logger::LogLevel::INFO, "Sending ERROR message for  packet "
                                                       + std::to_string(packet.getPSN())
                                                       + "of message ID "
                                                       + std::to_string(packetId)
                                                       + " back to source ID "
                                                       + std::to_string(packet.getSrcId())
                                                       + " (Broadcast).");
                                                         
            return; // Exit after sending the error message
        }

        // Handle message based on type
        // Determine the type of the message based on the packet ID
        switch (Message::getMessageTypeFromID(packet.getId())) {
            case DATA_MESSAGE:
                // If the message is a data message, handle it accordingly
                handleDataPacket(packet);
                break;
            case ERROR_MESSAGE:
                // If the message is an error message, handle it accordingly
                handleErrorPacket(packet);
                break;
            case ACK:
                // If the message is an acknowledgment message, handle it accordingly
                handleAckPacket(packet);
                break;
            default:
                // If the message type is unrecognized, do nothing
                break;
        }
    }
}

// Checks if the packet is intended for him
bool Communication::checkDestId(const Packet &packet)
{
    // If the packet is a broadcast, it can be received by any node
    //Or if the destination ID matches, the packet is intended for this node
    return packet.getIsBroadcast() || packet.getDestId() == processID;
}

// Checks if the data is correct
bool Communication::validCRC(const Packet &packet)
{
    // Validate the CRC of the packet
    // This checks the integrity of the packet to ensure it hasn't been corrupted
    return packet.validateCRC();
}

void Communication::handleDataPacket(const Packet &packet)
{
    RealSocket::log.logMessage(logger::LogLevel::INFO, "Handling data packet "
                                                        + std::to_string(packet.getPSN())
                                                        + " of message ID "
                                                        + std::to_string(packet.getId()) + ".");

    // Add the incoming data packet to the message queue
    addPacketToMessage(packet);

    // Retrieve the packet ID for acknowledgment
    uint32_t packetId = packet.getId();

    // Send an acknowledgment message back to the source of the packet
    RealSocket::log.logMessage(logger::LogLevel::INFO, "Sending ACK for packet "
                                                       + std::to_string(packet.getPSN())
                                                       + " of message ID "
                                                       + std::to_string(packetId)
                                                       + " back to source ID "
                                                       + std::to_string(packet.getSrcId()) + ".");
    sendMessage(&packetId, sizeof(packetId), packet.getSrcId(), packet.getDestId(), MessageType::ACK);
}

void Communication::handleErrorPacket(const Packet &packet)
{
    RealSocket::log.logMessage(logger::LogLevel::INFO, "Handling error packet "
                                                        + std::to_string(packet.getPSN())
                                                        + " of message ID "
                                                        + std::to_string(packet.getId()) + ".");

    // Check if the error packet is intended for this node
    if (packet.getDestId() == processID) {
        // Handle the transmission error for this node
        handleTransmitError();
    }
    else {
        // Handle the reception error for other nodes
        handleReceiveError();
    }
}

void Communication::handleAckPacket(const Packet &packet)
{
    RealSocket::log.logMessage(logger::LogLevel::INFO, "Handling ACK packet "
                                                        + std::to_string(packet.getPSN())
                                                        + " of message ID "
                                                        + std::to_string(packet.getId()) + ".");

    // Retrieve the message ID from the packet's payload
    const uint32_t *receivedIdPtr = reinterpret_cast<const uint32_t *>(packet.getPayload());
    uint32_t receivedId = *receivedIdPtr;

    // Notify the scheduler of the received acknowledgment
    scheduler.receiveACK(receivedId);

    // Handle the successful transmission
    handleSuccessfulTransmission();
}

// Adding the packet to the complete message
void Communication::addPacketToMessage(const Packet &packet)
{
    // Convert the packet ID to a string for lookup
    std::string messageId = std::to_string(packet.getId());

    // Check if the message with the given ID already exists
    if (receivedMessages.find(messageId) != receivedMessages.end()) {
        // If it exists, add the packet to the existing message
        RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Adding packet "
                                                            + std::to_string(packet.getPSN())
                                                            + " to existing message ID "
                                                            + messageId + ".");
        receivedMessages[messageId].addPacket(packet);
    }
    else {
        // If it does not exist, create a new message and add the packet
        RealSocket::log.logMessage(logger::LogLevel::INFO, "Creating a new message for receiving message ID "
                                                    + std::to_string(packet.getId())
                                                    + " and adding packet "
                                                    + std::to_string(packet.getPSN())
                                                    + " to it.");
        Message msg(packet.getTPS());
        msg.addPacket(packet);
        receivedMessages[messageId] = msg; // Store the new message
    }
    // If the message is complete, we pass the data to the passData function
    if (receivedMessages[messageId].isComplete()) {
        RealSocket::log.logMessage(logger::LogLevel::INFO, "Message ID "
                                                           + messageId
                                                           + " is complete. Processing complete data.");
        // Retrieve complete data from the message
        void *completeData = receivedMessages[messageId].completeData();
        // Pass the complete data for further processing
        passData(packet.getSrcId(), completeData);
        // Remove the message from the received messages
        receivedMessages.erase(messageId);
    }
}

void Communication::recoverFromBusOff()
{
    // Check if the communication system is in the BusOff state
    // and if the bus off recovery thread is not already running
    if (getState() == ErrorState::BusOff &&
        (!busOffRecoveryThread.joinable())) {
        RealSocket::log.logMessage(logger::LogLevel::INFO, "Recovering from BusOff state.");
        // Start a new thread for the recovery process
        busOffRecoveryThread = std::thread(&Communication::busOffTimer, this);
    }
}

void Communication::busOffTimer()
{
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "BusOff recovery started, sleeping for " + std::to_string(BUSOFF_RECOVERY_TIME.count()) + " milliseconds.");

    // Put the thread to sleep for the defined BusOff recovery time
    std::this_thread::sleep_for(BUSOFF_RECOVERY_TIME);

    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "BusOff recovery time elapsed. Resetting communication state.");
    // Reset the state of the communication system after the sleep period
    resetState();
}

void Communication::cleanupBusOffRecoveryThread()
{
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Cleaning up BusOff recovery thread.");
    // Check if the BusOff recovery thread is joinable
    if (busOffRecoveryThread.joinable()) {
        // Join the thread to clean up and ensure proper resource management
        busOffRecoveryThread.join();
    }
}

void Communication::checkState()
{
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Checking communication state. TEC: " + std::to_string(TEC) + ", REC: " + std::to_string(REC));

    // Lock the mutex to ensure thread safety while checking and updating the state
    {
        std::lock_guard<std::mutex> lock(mtx);

        // Check if TEC exceeds the BusOff threshold
        if (TEC >= THRESHOLD_TO_BUSOFF) {
            RealSocket::log.logMessage(logger::LogLevel::INFO, "State changed to BusOff. TEC exceeded threshold: " + std::to_string(TEC));
            state = ErrorState::BusOff; // Set state to BusOff
        }
        // Check if TEC or REC exceeds the Passive threshold
        else if (TEC >= THRESHOLD_TO_PASSIVE || REC >= THRESHOLD_TO_PASSIVE) {
            RealSocket::log.logMessage(logger::LogLevel::INFO, "State changed to Passive. TEC or REC exceeded threshold.");
            state = ErrorState::Passive; // Set state to Passive
        }
        else {
            RealSocket::log.logMessage(logger::LogLevel::DEBUG, "State remains Active.");
            state = ErrorState::Active; // Set state to Active
        }
    }

    // If the state is BusOff, initiate recovery process
    if (getState() == ErrorState::BusOff) {
        recoverFromBusOff();
    }
}

void Communication::handleTransmitError()
{
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Handling transmission error. Incrementing TEC by 8.");
    // Lock the mutex to ensure thread safety while updating the TEC
    {
        std::lock_guard<std::mutex> lock(mtx);
        TEC += 8; // Increment the Transmission Error Counter by 8
    }
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "TEC after increment: " + std::to_string(TEC));
    // Check the state based on the updated TEC
    checkState();
}

void Communication::handleACKError()
{
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Handling ACK error. Incrementing TEC by 1.");
    // Lock the mutex to ensure thread safety while updating the TEC
    {
        std::lock_guard<std::mutex> lock(mtx);
        TEC += 1; // Increment the Transmission Error Counter by 1
    }
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "TEC after increment: " + std::to_string(TEC));
    // Check the state based on the updated TEC
    checkState();
}

void Communication::handleReceiveError()
{
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Handling reception error. Incrementing REC by 1.");
    // Lock the mutex to ensure thread safety while updating the REC
    {
        std::lock_guard<std::mutex> lock(mtx);
        REC += 1; // Increment the Reception Error Counter by 1
    }
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "REC after increment: " + std::to_string(REC));
    // Check the state based on the updated REC
    checkState();
}

void Communication::handleSuccessfulTransmission()
{
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Handling successful transmission. Decrementing TEC by 1 if greater than zero.");
    // Lock the mutex to ensure thread safety while updating the TEC
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (TEC > 0)
            TEC -= 1; // Decrement the Transmit Error Counter by 1 if greater than zero
    }
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "TEC after decrement: " + std::to_string(TEC));
    // Check the state based on the updated TEC
    checkState();
}

void Communication::resetState()
{
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Resetting communication state. Resetting TEC, REC, and setting state to Active.");
    // Lock the mutex to ensure thread safety while resetting the state
    std::lock_guard<std::mutex> lock(mtx);
    TEC = 0;                    // Reset the Transmit Error Counter to zero
    REC = 0;                    // Reset the Receive Error Counter to zero
    state = ErrorState::Active; // Set the communication state to Active
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "State reset complete. TEC: 0, REC: 0, State: Active.");
}

ErrorState Communication::getState()
{
    // Lock the mutex to ensure thread safety while accessing the state
    std::lock_guard<std::mutex> lock(mtx);
    // Return the current state of the communication system
    return state; // Return the state regardless of its value
}

void Communication::ensureSingleSendPerTick()
{
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Ensuring single send per tick.");

    // Lock the mutex to ensure thread safety while checking the tick
    std::lock_guard<std::mutex> lock(sendMutex);

    // Retrieve the current clock tick from the GlobalClock
    int currentTick = GlobalClock::getCurrentTick();

    // Check if the current tick is different from the last sent tick
    if (currentTick != lastSendTick.load()) {
        // Update the last sent tick to the current tick
        lastSendTick.store(currentTick);

        RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Message can be sent this tick.");
        return; // Exit the function if a message can be sent
    }

    // Wait for the next clock tick if the last tick is the same
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Waiting for next clock tick.");
    GlobalClock::waitForNextTick();

    // Update lastSendTick after waiting
    lastSendTick.store(GlobalClock::getCurrentTick());
}

bool Communication::sendPacketWithRTO(Packet &packet, int retransmissions /* = 0 */)
{
    RealSocket::log.logMessage(logger::LogLevel::INFO, "Sending packet "
                                                        + std::to_string(packet.getPSN()) 
                                                        + ", with RTO of message ID: "
                                                        + std::to_string(packet.getId())
                                                        + ", Retransmissions: "
                                                        + std::to_string(retransmissions) + ".");

    // If the system is in BusOff state or retransmissions exceed the maximum limit, stop sending
    if (getState() == ErrorState::BusOff || retransmissions > MAX_RETRANSMISSIONS) {
        RealSocket::log.logMessage(logger::LogLevel::ERROR, "Cannot send packet "
                                                            + std::to_string(packet.getPSN())
                                                            + ",of message ID: "
                                                            + std::to_string(packet.getId())
                                                            + ", State: BusOff or retransmissions exceeded limit.");
        scheduler.clearPacketData(packet.getId());
        return false;
    }

    ensureSingleSendPerTick();

    // Set the packet to passive mode if the system state is passive
    packet.setIsPassive(getState() == ErrorState::Passive);

    // Attempt to send the packet using the client
    ErrorCode res = client.sendPacket(packet);

    // If the packet is successfully sent, wait for the ACK
    if (res == ErrorCode::SUCCESS) {
        RealSocket::log.logMessage(logger::LogLevel::INFO, "Packet "
                                                            + std::to_string(packet.getPSN())
                                                            + ", of message ID: " + std::to_string(packet.getId())
                                                            + " sent. Waiting for ACK...");

        // Create a promise to track the success of the transmission (ACK reception)
        auto ackPromisePtr = std::make_shared<std::promise<bool>>();
        auto ackFuture = ackPromisePtr->get_future(); // Future to wait for the ACK result

        // Start a retransmission timer for this packet
        scheduler.startRetransmissionTimer(
            packet.getId(),
            [this, packet, ackPromisePtr](int retryCount) mutable {
                // Handle errors if ACK was not received
                this->handleACKError();

                // Retry sending the packet and update the promise with the result
                bool result = this->sendPacketWithRTO(packet, retryCount);
                ackPromisePtr->set_value(result); // Set the result of retransmission
            },
            ackPromisePtr);

        // Wait for the result of the future (ACK reception or timeout)
        bool success = ackFuture.get();

        RealSocket::log.logMessage(logger::LogLevel::INFO,
                            std::string(success ? "ACK received successfully" : "ACK reception failed")
                            + " for packet "
                            + std::to_string(packet.getPSN())
                            + ", of message ID: "
                            + std::to_string(packet.getId()) + ".");
                                   
        return success; // Return true if successful, false otherwise
    }

    RealSocket::log.logMessage(logger::LogLevel::ERROR, "Packet "
                                                        + std::to_string(packet.getPSN())
                                                        + ", of message ID: " + std::to_string(packet.getId())
                                                        + " transmission failed.");
    // Return false if the packet transmission failed
    return false;
}

bool Communication::sendPacket(Packet &packet)
{
    RealSocket::log.logMessage(logger::LogLevel::INFO, "Sending packet "
                                                       + std::to_string(packet.getPSN())
                                                       + ", of message ID: "
                                                       + std::to_string(packet.getId()) + ".");

    // Check if the system is in BusOff state
    if (getState() == ErrorState::BusOff) {
        RealSocket::log.logMessage(logger::LogLevel::ERROR, "Cannot send packet "
                                                            + std::to_string(packet.getPSN())
                                                            + ",of message ID: "
                                                            + std::to_string(packet.getId())
                                                            + ", system in BusOff state.");
        return false; // Return false immediately
    }

    ensureSingleSendPerTick();

    // Set the packet to passive mode if the system state is passive
    packet.setIsPassive(getState() == ErrorState::Passive);

    // Attempt to send the packet using the client
    ErrorCode res = client.sendPacket(packet);

    res == ErrorCode::SUCCESS ?
    RealSocket::log.logMessage(logger::LogLevel::INFO, "Packet "
                                                       + std::to_string(packet.getPSN())
                                                       + ", of message ID: "
                                                       + std::to_string(packet.getId())
                                                       + " sent successfully."):
     RealSocket::log.logMessage(logger::LogLevel::ERROR, "Packet "
                                                       + std::to_string(packet.getPSN())
                                                       + ", of message ID: "
                                                       + std::to_string(packet.getId())
                                                       + " Failed to send.");                                                   
    return res == ErrorCode::SUCCESS; // Return true if successful, false otherwise
}

void Communication::waitForActiveMessages()
{
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "Waiting for active messages to complete...");

    std::unique_lock<std::mutex> lock(messageThreadsMutex); // Lock for thread safety

    // Wait for all active message threads to complete
    for (auto &future : activeMessageFutures) {
        if (future.valid()) {
            future.wait(); // Wait for the future to finish if valid
        }
    }

    activeMessageFutures.clear(); // Clear the vector after all futures are completed
    RealSocket::log.logMessage(logger::LogLevel::DEBUG, "All active messages completed.");
}

void Communication::signalHandler(int signum)
{
    if (instance) {
        cleanupBusOffRecoveryThread(); // Clean up the recovery thread if it is running
        waitForActiveMessages(); // Wait for any active message threads to complete
        instance->client.closeConnection(); // Close the client connection
    }
    exit(signum); // Exit the program with the given signal number
}

void Communication::setId(uint32_t id)
{
    processID = id;
}

void Communication::setPassDataCallback(void (*callback)(uint32_t, void *))
{
    if (callback == nullptr)
        throw std::invalid_argument("Invalid callback function: passDataCallback cannot be null");

    this->passData = callback;
}

void Communication::setSignaleHandler()
{
#ifndef ESP32
    auto signalResult = signal(SIGINT, Communication::signalHandler);
    if (signalResult == SIG_ERR)
        throw std::runtime_error("Failed to set signal handler for SIGINT");
#else
    receiveTaskHandle = nullptr; // Initialize FreeRTOS task handle
#endif
}

//Destructor
Communication::~Communication()
{
#ifdef ESP32
    if (receiveTaskHandle != nullptr) {
        vTaskDelete(receiveTaskHandle);
    }
#endif
    cleanupBusOffRecoveryThread(); // Clean up the bus off recovery thread
    instance->client.closeConnection(); // Close the client connection
    instance = nullptr;                 // Clear the singleton instance
}