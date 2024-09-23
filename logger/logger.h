#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>

#ifdef ESP32
#include <Arduino.h>
#else
#include <fstream>
#include <mutex>
#endif

#ifndef LOG_LVL
    #define LOG_LVL 1
#endif

#if LOG_LVL == 0
    #define LOG_LEVEL logger::LogLevel::ERROR
#elif LOG_LVL == 1
    #define LOG_LEVEL logger::LogLevel::INFO
#elif LOG_LVL == 2
    #define LOG_LEVEL logger::LogLevel::DEBUG
#else
    #define LOG_LEVEL logger::LogLevel::INFO
#endif

class logger {
public:
    enum class LogLevel {
        ERROR,
        INFO,
        DEBUG,
    };

    logger() {}
    logger(std::string componentName);
    
    void logMessage(LogLevel level, const std::string &message);
    void logMessage(LogLevel level, std::string src, std::string dst, const std::string &message);
    
    void initializeLogFile();
    std::string getLogFileName();
    void cleanUp();
    
    static std::string sharedLogFileName;

private:
    static std::string logLevelToString(LogLevel level);
    static bool shouldLog(LogLevel level);
    static std::string getElapsedTime();
    static std::string componentName;
    static std::string logFileName;
    bool isInitialized = false;

#ifdef ESP32
    static SemaphoreHandle_t logMutex;
    static uint64_t initTime;
#else
    static std::mutex logMutex;
    static std::chrono::system_clock::time_point initTime;
#endif
};

#endif // LOGGER_H
