#include "../include/communication.h"

// Initialize static member variables for the Communication class
Communication *Communication::instance = nullptr;
std::thread Communication::busOffRecoveryThread;

// Constructor
Communication::Communication(uint32_t id, void (*passDataCallback)(uint32_t, void *))
    : TEC(0), REC(0), state(ErrorState::Active),
      client(std::bind(&Communication::receivePacket, this,
                       std::placeholders::_1))
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
    RealSocket::log.logMessage(logger::LogLevel::INFO,std::to_string(srcID),std::to_string(destID),"Complete message:" + msg.getPackets().at(0).pointerToHex(data, dataSize));
    
    for (auto &packet : msg.getPackets()) {
#ifdef ESP32
          delay(80);
#endif
        bool success =
            (messageType == MessageType::DATA_MESSAGE)
                ? sendPacketWithRTO(packet) // Send with retransmission timeout
                : sendPacket(packet);       // Send packet directly
        if (!success)
            return ErrorCode::SEND_FAILED; // Return failure if sending fails
    }
    return ErrorCode::SUCCESS; // Return success if all packets are sent
}       

// Send message asynchronously
void Communication::sendMessageAsync(
    void *data, size_t dataSize, uint32_t destID, uint32_t srcID,
    std::function<void(ErrorCode)> sendCallback, MessageType messageType)
{
    
}

void Communication::receivePacket(Packet &packet)
{
    // Check if the bus is in BusOff state
    // If the system is in BusOff state, exit the function and do not process the packet
    if (getState() == ErrorState::BusOff)
        return;

    // Check if the destination ID of the packet matches the expected destination
    if (checkDestId(packet)) {
        // Check CRC validity
        // Validate the packet's CRC for data integrity
        if (!validCRC(packet)) {
            // If CRC is invalid, retrieve the packet ID
            uint32_t packetId = packet.getId();
            // Send an error message back to the source with the invalid packet ID
            sendMessage(&packetId, sizeof(packetId), packet.getSrcId(),
                        packet.getDestId(), MessageType::ERROR_MESSAGE);
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
bool Communication::checkDestId(Packet &p)
{
    // If the packet is a broadcast, it can be received by any node
    //Or if the destination ID matches, the packet is intended for this node
    return p.getIsBroadcast() || p.getDestId() == processID;
}

// Checks if the data is correct
bool Communication::validCRC(const Packet &p)
{
    // Validate the CRC of the packet
    // This checks the integrity of the packet to ensure it hasn't been corrupted
    return packet.validateCRC();
}

void Communication::handleDataPacket(Packet &packet)
{
    // Add the incoming data packet to the message queue
    addPacketToMessage(packet);

    // Retrieve the packet ID for acknowledgment
    uint32_t packetId = packet.getId();

    // Send an acknowledgment message back to the source of the packet
    sendMessage(&packetId, sizeof(packetId), packet.getSrcId(),
                packet.getDestId(), MessageType::ACK);
}

void Communication::handleErrorPacket(Packet &packet)
{
    // Check if the error packet is intended for this node
    if (packet.getDestId() == id) {
        // Handle the transmission error for this node
        handleTransmitError();
    }
    else {
        // Handle the reception error for other nodes
        handleReceiveError();
    }
}

void Communication::handleAckPacket(Packet &packet)
{
    // Retrieve the message ID from the packet's payload
    const uint32_t *receivedIdPtr =
        reinterpret_cast<const uint32_t *>(packet.getPayload());
    uint32_t receivedId = *receivedIdPtr;

    // Notify the scheduler of the received acknowledgment
    scheduler.receiveACK(receivedId);

    // Handle the successful transmission
    handleSuccessfulTransmission();
}

// Adding the packet to the complete message
void Communication::addPacketToMessage(Packet &packet)
{
    // Convert the packet ID to a string for lookup
    std::string messageId = std::to_string(packet.getId());

    // Check if the message with the given ID already exists
    if (receivedMessages.find(messageId) != receivedMessages.end()) {
        // If it exists, add the packet to the existing message
        receivedMessages[messageId].addPacket(packet);
    }
    else {
        // If it does not exist, create a new message and add the packet
        Message msg(packet.getTPS());
        msg.addPacket(packet);
        receivedMessages[messageId] = msg; // Store the new message
    }
    // If the message is complete, we pass the data to the passData function
    if (receivedMessages[messageId].isComplete()) {
        // Retrieve complete data from the message
        void *completeData = receivedMessages[messageId].completeData();
        // Pass the complete data for further processing
        passData(packet.getSrcId(),completeData);
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
        // Start a new thread for the recovery process
        busOffRecoveryThread = std::thread(&Communication::busOffTimer, this);
    }
}

void Communication::busOffTimer()
{
    // Put the thread to sleep for the defined BusOff recovery time
    std::this_thread::sleep_for(BUSOFF_RECOVERY_TIME);
    // Reset the state of the communication system after the sleep period
    resetState();
}

void Communication::cleanupBusOffRecoveryThread()
{
    // Check if the BusOff recovery thread is joinable
    if (busOffRecoveryThread.joinable()) {
        // Join the thread to clean up and ensure proper resource management
        busOffRecoveryThread.join();
    }
}

void Communication::checkState()
{
    // Lock the mutex to ensure thread safety while checking and updating the state
    {
        std::lock_guard<std::mutex> lock(mtx);
        // Check if TEC exceeds the BusOff threshold
        if (TEC >= THRESHOLD_TO_BUSOFF) {
            state = ErrorState::BusOff; // Set state to BusOff
        }
        // Check if TEC or REC exceeds the Passive threshold
        else if (TEC >= THRESHOLD_TO_PASSIVE || REC >= THRESHOLD_TO_PASSIVE) {
            state = ErrorState::Passive; // Set state to Passive
        }
        else {
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
    // Lock the mutex to ensure thread safety while updating the TEC
    {
        std::lock_guard<std::mutex> lock(mtx);
        TEC += 8; // Increment the Transmission Error Counter by 8
    }
    // Check the state based on the updated TEC
    checkState();
}

void Communication::handleACKError(Packet &packet)
{
    // Lock the mutex to ensure thread safety while updating the TEC
    {
        std::lock_guard<std::mutex> lock(mtx);
        TEC += 1; // Increment the Transmission Error Counter by 1
    }
    // Check the state based on the updated TEC
    checkState();
}

void Communication::handleReceiveError()
{
    // Lock the mutex to ensure thread safety while updating the REC
    {
        std::lock_guard<std::mutex> lock(mtx);
        REC += 1; // Increment the Reception Error Counter by 1
    }
    // Check the state based on the updated REC
    checkState();
}

void Communication::handleSuccessfulTransmission()
{
    // Lock the mutex to ensure thread safety while updating the TEC
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (TEC > 0)
            TEC -=
                1; // Decrement the Transmit Error Counter by 1 if greater than zero
    }
    // Check the state based on the updated TEC
    checkState();
}

void Communication::resetState()
{
    // Lock the mutex to ensure thread safety while resetting the state
    std::lock_guard<std::mutex> lock(mtx);
    TEC = 0;                    // Reset the Transmit Error Counter to zero
    REC = 0;                    // Reset the Receive Error Counter to zero
    state = ErrorState::Active; // Set the communication state to Active
}

ErrorState Communication::getState()
{
    // Lock the mutex to ensure thread safety while accessing the state
    std::lock_guard<std::mutex> lock(mtx);
    // Return the current state of the communication system
    return state; // Return the state regardless of its value
}

bool Communication::sendPacketWithRTO(Packet &packet,
                                      int retransmissions /* = 0 */)
{
    // If the system is in BusOff state or retransmissions exceed the maximum limit, stop sending
    if (getState() == ErrorState::BusOff ||
        retransmissions > MAX_RETRANSMISSIONS) {
        // Clear any packet-related data (if needed) if we stop the transmission
        scheduler.clearPacketData(packet.getId());
        return false;
    }

    // Set the packet to passive mode if the system state is passive (for error handling)
    packet.setIsPassive(getState() == ErrorState::Passive);

    // Attempt to send the packet using the client
    ErrorCode res = client.sendPacket(packet);

    // If the packet is successfully sent, wait for the ACK
    if (res == ErrorCode::SUCCESS) {
        // Create a promise to track the success of the transmission (ACK reception)
        auto ackPromisePtr = std::make_shared<std::promise<bool>>();
        auto ackFuture =
            ackPromisePtr->get_future(); // Future to wait for the ACK result

        // Start a retransmission timer for this packet
        scheduler.startRetransmissionTimer(
            packet.getId(),
            [this, packet, ackPromisePtr](int retryCount) mutable {
                // Handle errors if ACK was not received
                this->handleACKError(packet);

                // Retry sending the packet and update the promise with the result
                bool result = this->sendPacketWithRTO(packet, retryCount);
                ackPromisePtr->set_value(
                    result); // Set the result of retransmission
            },
            ackPromisePtr);

        // Wait for the result of the future (ACK reception or timeout)
        bool success = ackFuture.get();

        return success; // Return true if successful, false otherwise
    }

    // Return false if the initial packet transmission failed
    return false;
}

bool Communication::sendPacket(Packet &packet)
{
    // Check if the system is in BusOff state
    if (getState() == ErrorState::BusOff)
        return false; 

    // Set the packet to passive mode if the system state is passive
    packet.setIsPassive(getState() == ErrorState::Passive);

    // Attempt to send the packet using the client
    ErrorCode res = client.sendPacket(packet);

    return res ==
           ErrorCode::SUCCESS; // Return true if successful, false otherwise
}

void Communication::signalHandler(int signum)
{
    if (instance) {
        cleanupBusOffRecoveryThread(); // Clean up the recovery thread if it is running
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