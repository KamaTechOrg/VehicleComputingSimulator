#pragma once
#include <thread>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <functional>
#include <iomanip>
#include "../src/message.h"

enum LogLevel {
    INFO,
    ERROR,
    DEBUG
};

// Convert a LogLevel to its string representation.
inline std::string logLevelToString(LogLevel level) {
    switch (level) {
        case INFO:
            return "INFO";
        case ERROR:
            return "ERROR";
        case DEBUG:
            return "DEBUG";
        default:
            return "UNKNOWN";
    }
}

class Logger {
public:
    
    // Path to the log file
    static const std::string LOG_FILE;

    // Log a general message
    static void generalLogMessage(const std::string& ID, LogLevel logLevel, const std::string& message);
    
    // Log a packet message
    static void packetLogMessage(const std::string& ID, LogLevel logLevel, const std::string& message, const Packet& packet);
};
