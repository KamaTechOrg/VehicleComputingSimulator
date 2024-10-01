#ifndef NETWORKINFO_H
#define NETWORKINFO_H

#include <string>
#include<unordered_map>
#include<vector>
#include <cstdint>
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <array>
#include <fstream>
#include <nlohmann/json.hpp>
#include "error_code.h"

class NetworkInfo 
{
private:
    static std::vector<uint16_t> ports;
    static std::vector<uint32_t> processIds;
    static int maxCriticalProcessID;

public:
    static std::vector<uint16_t> getPorts();

    static std::vector<uint32_t> getProcessIds();

    static int getMaxCriticalProcessID();

    // Initializing the ports of the busses
    static std::unordered_map<uint32_t, uint16_t> assignPorts(std::vector<uint32_t>& ids);

    // Checking if the port is free
    static bool isPortAvailable(uint16_t port);

    static std::string getLocalIPAddress();

    // Function to load the process configuration (IDs and ports) from a JSON file
    static ErrorCode loadProcessConfig(const std::string& filePath);
};

#endif