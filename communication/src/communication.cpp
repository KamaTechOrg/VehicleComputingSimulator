#include "../include/communication.h"

Communication* Communication::instance = nullptr;

// Constructor
Communication::Communication(uint32_t id, void (*passDataCallback)(uint32_t, void *)) : 
    client(std::bind(&Communication::receivePacket, this, std::placeholders::_1))
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

// Sends a message sync
ErrorCode Communication::sendMessage(void *data, size_t dataSize, uint32_t destID, uint32_t srcID, bool isBroadcast)
{
    if (dataSize == 0)
        return ErrorCode::INVALID_DATA_SIZE;

    if (data == nullptr)
        return ErrorCode::INVALID_DATA;

    if (!client.isConnected())
        return ErrorCode::CONNECTION_FAILED;

    Message msg(srcID, data, dataSize, isBroadcast, destID);
    
    //Sending the message to logger
    //RealSocket::log.logMessage(logger::LogLevel::INFO,std::to_string(srcID),std::to_string(destID),"Complete message:" + msg.getPackets().at(0).pointerToHex(data, dataSize));
    
    for (auto &packet : msg.getPackets()) {
        ErrorCode res = client.sendPacket(packet);
#ifdef ESP32
          delay(80);
#endif
        if (res != ErrorCode::SUCCESS)
            return res;
    }

    return ErrorCode::SUCCESS;  
}

// Sends a message Async
void Communication::sendMessageAsync(void *data, size_t dataSize, uint32_t destID, uint32_t srcID, std::function<void(ErrorCode)> sendCallback, bool isBroadcast)
{
#ifdef ESP32
    // Create a FreeRTOS task for asynchronous message sending
    xTaskCreate([](void* param) {
        auto* asyncParams = static_cast<std::tuple<Communication*, void*, size_t, uint32_t, uint32_t, bool, std::function<void(ErrorCode)> >*>(param);
        Communication* comm = std::get<0>(*asyncParams);
        void* data = std::get<1>(*asyncParams);
        size_t dataSize = std::get<2>(*asyncParams);
        uint32_t destID = std::get<3>(*asyncParams);
        uint32_t srcID = std::get<4>(*asyncParams);
        bool isBroadcast = std::get<5>(*asyncParams);
        std::function<void(ErrorCode)> callback = std::get<6>(*asyncParams);

        ErrorCode res = comm->sendMessage(data, dataSize, destID, srcID, isBroadcast);
        callback(res);

        delete asyncParams; // Clean up the parameters
        vTaskDelete(nullptr); // Delete the FreeRTOS task
    }, "SendMessageTask", 2048, new std::tuple<Communication*, void*, size_t, uint32_t, uint32_t, bool, std::function<void(ErrorCode)> >(this, data, dataSize, destID, srcID, isBroadcast, sendCallback), 1, nullptr);
#else
    std::promise<ErrorCode> resultPromise;
    std::future<ErrorCode> resultFuture = resultPromise.get_future();

    std::thread([this, data, dataSize, destID, srcID, isBroadcast, &resultPromise]() {
        ErrorCode res = this->sendMessage(data, dataSize, destID, srcID, isBroadcast);
        resultPromise.set_value(res);
    }).detach();
    
    ErrorCode res = resultFuture.get();
    sendCallback(res);
#endif
}

// Accepts the packet from the client and checks..
void Communication::receivePacket(Packet &p)
{
    if (checkDestId(p)) {
        if (validCRC(p))
            handlePacket(p);
        else
            handleError();
    }
}

// Checks if the packet is intended for him
bool Communication::checkDestId(const Packet &p)
{
    return p.header.isBroadcast || p.header.DestID == processID;
}

// Checks if the data is correct
bool Communication::validCRC(const Packet &p)
{
    return p.header.CRC == p.calculateCRC(p.data, p.header.DLC);
}

// Receives the packet and adds it to the message
void Communication::handlePacket(const Packet &p)
{
    // Send acknowledgment according to CAN bus
    // client.sendPacket(hadArrived());
    addPacketToMessage(p);
}

// Implement error handling according to CAN bus
void Communication::handleError()
{
    // Handle error cases according to CAN bus
}

// Implement arrival confirmation according to the CAN bus
Packet Communication::hadArrived()
{
    Packet ack;
    // Construct and return acknowledgment packet
    return ack;
}

// Adding the packet to the complete message
void Communication::addPacketToMessage(const Packet &p)
{
    std::string messageId = std::to_string(p.header.ID);
    if (receivedMessages.find(messageId) != receivedMessages.end()) {
        receivedMessages[messageId].addPacket(p);
    } else {
        Message msg(p.header.TPS);
        msg.addPacket(p);
        receivedMessages[messageId] = msg;
    }

    if (receivedMessages[messageId].isComplete()) {
        void *completeData = receivedMessages[messageId].completeData();
        passData(p.header.SrcID, completeData);
        receivedMessages.erase(messageId); // Removing the message once completed
    }
}

// Static method to handle SIGINT signal
void Communication::signalHandler(int signum)
{
    if (instance)
        instance->client.closeConnection();
    
    exit(signum);
}

void Communication::setId(uint32_t id)
{
    processID = id;
}

void Communication::setPassDataCallback(void (*callback)(uint32_t, void *))
{
    if (callback == nullptr)
        throw std::invalid_argument("Invalid callback function: passDataCallback cannot be null");
    passData = callback;
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
Communication::~Communication() {
    instance = nullptr;
#ifdef ESP32
    if (receiveTaskHandle != nullptr) {
        vTaskDelete(receiveTaskHandle);
    }
#endif
}