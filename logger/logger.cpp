#include "logger.h"

// Path to the log file
const std::string Logger::LOG_FILE="communication.log";

// Log a general message
void Logger::generalLogMessage(const std::string &ID, LogLevel logLevel, const std::string &message)
{
    std::ofstream logFile(LOG_FILE, std::ios_base::app);
    if (!logFile){
        std::cerr << "[ERROR] Failed to open log file" << std::endl;
        return;
    }
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time);

    logFile << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] "
            << "ID Process: " << ID << " " << logLevelToString(logLevel) << " " << message << std::endl;
}

// Log a packet message
void Logger::packetLogMessage(const std::string &ID, LogLevel logLevel, const std::string &message, const Packet &packet)
{
    std::ofstream logFile(LOG_FILE, std::ios_base::app);
    if (!logFile){
        std::cerr << "[ERROR] Failed to open log file" << std::endl;
        return;
    }
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time);

    logFile << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] "
            << "ID Process: " << ID << " " << logLevelToString(logLevel) << " " << message
            << " from " << packet.header.SrcID << " to " << packet.header.DestID
            << " Sequence Number " << packet.header.PSN << std::endl;
}