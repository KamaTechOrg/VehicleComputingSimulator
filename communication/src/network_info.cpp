#include "../include/network_info.h"

// Initializing static members
std::vector<uint16_t> NetworkInfo::ports;
std::vector<uint32_t> NetworkInfo::processIds;
int NetworkInfo::maxCriticalProcessID;

// Initializing the ports of the busses
std::unordered_map<uint32_t, uint16_t> NetworkInfo::assignPorts(std::vector<uint32_t> &ids)
{
    std::unordered_map<uint32_t, uint16_t> idToPort;
    int basePort = 8080;
    for (const int id : ids) {
        while (!isPortAvailable(basePort))
            basePort++;
        
        ports.push_back(basePort);
        idToPort[id] = basePort;
        basePort++;
    }

    return idToPort;
}

// Checking if the port is free
bool NetworkInfo::isPortAvailable(uint16_t port)
{
    std::string command = "netstat -an | grep :" + std::to_string(port) + " > /dev/null";
    int result = std::system(command.c_str());
    
    return (result != 0);
}

// Function to retrieve the local machine's IP address
std::string NetworkInfo::getLocalIPAddress()
{
    std::string result;
    std::string command = "cmd.exe /c ipconfig"; // Use cmd to run the ipconfig command

    // Execute the command and read the output
    std::array<char, 128> buffer;
    std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);

    if (!pipe) throw std::runtime_error("popen() failed!");

    // Read the output line by line
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        // Append the output to the result string
        result += buffer.data(); 

    // Search for the IP address in the Wi-Fi adapter section
    size_t pos = result.find("Wireless LAN adapter Wi-Fi:");
    if (pos != std::string::npos) {
        // Find the IPv4 Address line in the Wi-Fi section
        pos = result.find("IPv4 Address", pos);
        if (pos != std::string::npos) {
            pos += 36; 
            size_t end = result.find('\n', pos); 
            std::string ip = result.substr(pos, end - pos); 
            return ip; 
        }
    }

    // Return an empty string if the IP address was not found
    return ""; 
}

// Function to load the process configuration (IDs and ports) from a JSON file
ErrorCode NetworkInfo::loadProcessConfig(const std::string& filePath)
{
    // Open the JSON file
    std::ifstream file(filePath);
    if (!file.is_open())
        return ErrorCode::FILE_OPEN_FAILED;

    // Parse the JSON file
    nlohmann::json jsonDoc;
    try {
        file >> jsonDoc;
    } catch (const nlohmann::json::parse_error& e) {
        return ErrorCode::JSON_PARSE_FAILED;
    }

    // Check if the processes array is present
    if (! jsonDoc.contains("processIds") && jsonDoc["processIds"].is_array())
        return ErrorCode::MISSING_KEYS;
    for (const auto& process : jsonDoc["processIds"])
        processIds.push_back(process.get<uint32_t>());

    // Check if the maxCriticalProcessID is present
    if (! jsonDoc.contains("maxCriticalProcessID"))
        return ErrorCode::MISSING_KEYS;
    maxCriticalProcessID = jsonDoc ["maxCriticalProcessID"].get<int>();
    
    file.close();
    return ErrorCode::SUCCESS;
}

std::vector<uint16_t> NetworkInfo::getPorts()
{
    return ports;
}

std::vector<uint32_t> NetworkInfo::getProcessIds()
{
    return processIds;
}

int NetworkInfo::getMaxCriticalProcessID()
{
    return maxCriticalProcessID;
}
