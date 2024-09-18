#ifndef I_MANAGER
#define I_MANAGER

#pragma once
#include "packet.h"
#include <vector>
#include <cstdint>
#include "error_code.h"

class IManager {
public:
    virtual ~IManager() = default;

    virtual ErrorCode receiveMessage(Packet &packet) = 0;

    virtual ErrorCode receiveNewProcessID(const uint32_t processID ,const uint32_t port) = 0;

    virtual ErrorCode sendMessage(const Packet &packet) = 0;

    virtual ErrorCode startConnection() = 0;

    virtual ErrorCode closeConnection() = 0;
};
#endif