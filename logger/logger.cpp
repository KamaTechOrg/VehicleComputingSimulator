#include <sys/stat.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <sstream>
#include "logger.h"


std::string logger::logFileName;
std::mutex logger::logMutex;
std::chrono::system_clock::time_point logger::initTime =
    std::chrono::system_clock::now();
std::string logger::componentName = "out";
std::string logger::logDirectory = "./logs/";

logger::logger(std::string componentName)
{
    logger::componentName = componentName;
}

// Function to create the log directory if it doesn't exist
void logger::createLogDirectory() {
    struct stat info;
    if (stat(logDirectory.c_str(), &info) != 0) {
        // Directory doesn't exist - create it
        #ifdef _WIN32
            _mkdir(logDirectory.c_str());
        #else
            mkdir(logDirectory.c_str(), 0755);  // Linux/Unix permissions
        #endif
    }
}

void logger::initializeLogFile()
{
    if (isInitialized)
        return;

    // Create the directory (if it doesn't exist)
    createLogDirectory();

    auto time = std::chrono::system_clock::to_time_t(initTime);
    std::tm tm = *std::localtime(&time);

    std::ostringstream oss;
    oss << logDirectory << std::put_time(&tm, "%Y_%m_%d_%H_%M_%S") << ".log";
    logFileName = oss.str();

    std::ofstream sharedFile(sharedLogFileName,
                             std::ios::out | std::ios::trunc);
    if (sharedFile) {
        sharedFile << logFileName;
    }
    else {
        std::cerr << logLevelToString(LogLevel::ERROR)
                  << "Failed to open shared log file name file" << std::endl;
    }

    isInitialized = true;
}

std::string logger::getLogFileName()
{
    if (!isInitialized) {
        if (!isInitialized) {
            std::ifstream sharedFile(sharedLogFileName);
            if (sharedFile) {
                std::getline(sharedFile, logFileName);
            }
            if (logFileName.empty()) {
                initializeLogFile();
            }
        }
    }

    return logFileName;
}

void logger::cleanUp()
{
    std::remove(sharedLogFileName.c_str());
}

std::string logger::logLevelToString(LogLevel level)
{
    switch (level) {
        case LogLevel::ERROR:
            return "[ERROR]";
        case LogLevel::INFO:
            return "[INFO]";
        case LogLevel::DEBUG:
            return "[DEBUG]";
        default:
            return "[UNKNOWN]";
    }
}

bool logger::shouldLog(LogLevel level)
{
    switch (LOG_LEVEL) {
        case LogLevel::ERROR:
            return level == LogLevel::ERROR;
        case LogLevel::INFO:
            return level == LogLevel::ERROR || level == LogLevel::INFO;
        case LogLevel::DEBUG:
            return true;
        default:
            return false;
    }
}

std::string logger::getElapsedTime()
{
    auto now = std::chrono::system_clock::now();
    auto elapsed =
        std::chrono::duration_cast<std::chrono::nanoseconds>(now - initTime)
            .count();
    return std::to_string(elapsed) + "ns";
}

void logger::logMessage(LogLevel level, std::string src, std::string dst,
                        const std::string &message)
{
    if (!shouldLog(level))
        return;

    std::lock_guard<std::mutex> guard(logMutex);
    std::ofstream logFile(getLogFileName(), std::ios_base::app);
    if (!logFile) {
        std::cerr << logLevelToString(LogLevel::ERROR)
                  << "Failed to open log file" << std::endl;
        return;
    }
    logFile << "[" << getElapsedTime() << "] " << logLevelToString(level) << " "
            << "SRC " << src << " "
            << "DST " << dst << " " << message << std::endl;
}

void logger::logMessage(LogLevel level, const std::string &message)
{
    if (!shouldLog(level))
        return;

    std::lock_guard<std::mutex> guard(logMutex);

    std::ofstream logFile(getLogFileName(), std::ios_base::app);
    if (!logFile) {
        std::cerr << logLevelToString(LogLevel::ERROR)
                  << "Failed to open log file" << std::endl;
        return;
    }

    logFile << "[" << getElapsedTime() << "] " << logLevelToString(level) << " "
            << message << std::endl;
}