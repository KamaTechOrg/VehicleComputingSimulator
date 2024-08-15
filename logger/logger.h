#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <iostream>
#include <mutex>

#ifndef LOG_LEVEL
#define LOG_LEVEL logger::LogLevel::INFO
#endif

class logger {
public:
    enum class LogLevel {
        ERROR,
        INFO,
        DEBUG,
    };

    static void logMessage(LogLevel level, std::string src, std::string dst, const std::string &message);
    static void initializeLogFile();
    static std::string getLogFileName();

private:
    static std::string logLevelToString(LogLevel level);
    static bool shouldLog(LogLevel level);
    static std::string getElapsedTime();

    static std::string logFileName;
    static bool isInitialized;
    static std::mutex logMutex;
    static std::chrono::system_clock::time_point initTime;
};

#endif // LOGGER_H
