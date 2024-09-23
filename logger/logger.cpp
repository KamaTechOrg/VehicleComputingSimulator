#include "logger.h"

#ifdef ESP32
#include <Arduino.h> // Required for Serial

std::string logger::logFileName;
SemaphoreHandle_t logger::logMutex = xSemaphoreCreateMutex();
uint64_t logger::initTime = esp_timer_get_time(); // Start time in microseconds
std::string logger::componentName = "out";
#else
// For other platforms (without ESP32)
std::string logger::logFileName;
std::string logger::sharedLogFileName = "shared_log_file_name.txt";
std::mutex logger::logMutex;
std::chrono::system_clock::time_point logger::initTime = std::chrono::system_clock::now();
std::string logger::componentName = "out";
#endif

logger::logger(std::string componentName)
{
    logger::componentName = componentName;
}

#ifdef ESP32
void logger::initializeLogFile()
{
    // Do nothing on ESP32 when not writing to a file
}
#else
void logger::initializeLogFile()
{
    if (isInitialized) return;

    auto time = std::chrono::system_clock::to_time_t(initTime);
    std::tm tm = *std::localtime(&time);

    std::ostringstream oss;
    oss << "" << std::put_time(&tm, "%Y_%m_%d_%H_%M_%S") << "_" << componentName << ".log";
    logFileName = oss.str();

    std::ofstream sharedFile(sharedLogFileName, std::ios::out | std::ios::trunc);
    if (sharedFile) {
        sharedFile << logFileName;
    } else {
        std::cerr << logLevelToString(LogLevel::ERROR) << "Failed to open shared log file name file" << std::endl;
    }

    isInitialized = true;
}
#endif

std::string logger::getLogFileName()
{
#ifdef ESP32
    return ""; // No file name on ESP32
#else
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
#endif
}

void logger::cleanUp()
{
#ifndef ESP32
    std::remove(sharedLogFileName.c_str());
#endif
}

std::string logger::logLevelToString(LogLevel level)
{
    switch (level) {
        case LogLevel::ERROR: return "[ERROR]";
        case LogLevel::INFO: return "[INFO]";
        case LogLevel::DEBUG: return "[DEBUG]";
        default: return "[UNKNOWN]";
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
#ifdef ESP32
    uint64_t now = esp_timer_get_time();
    uint64_t elapsed = now - initTime; // Time since the start in microseconds
    return std::to_string(elapsed) + "us";
#else
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - initTime).count();
    return std::to_string(elapsed) + "ns";
#endif
}

void logger::logMessage(LogLevel level, std::string src, std::string dst, const std::string &message)
{
#ifdef ESP32
    if (xSemaphoreTake(logMutex, portMAX_DELAY)) {
        Serial.printf("[%s] %s SRC %s DST %s %s\n",
                      getElapsedTime().c_str(),
                      logLevelToString(level).c_str(),
                      src.c_str(),
                      dst.c_str(),
                      message.c_str());
        xSemaphoreGive(logMutex);
    }
#else
    if (!shouldLog(level)) return;

    std::lock_guard<std::mutex> guard(logMutex);
    std::ofstream logFile(getLogFileName(), std::ios_base::app);
    if (!logFile) {
        std::cerr << logLevelToString(LogLevel::ERROR) << "Failed to open log file" << std::endl;
        return;
    }
    logFile << "[" << getElapsedTime() << "] "
            << logLevelToString(level) << " "
            << "SRC " << src << " "
            << "DST " << dst << " "
            << message << std::endl;
#endif
}

void logger::logMessage(LogLevel level, const std::string &message)
{
#ifdef ESP32
    if (xSemaphoreTake(logMutex, portMAX_DELAY)) {
        Serial.printf("[%s] %s %s\n",
                      getElapsedTime().c_str(),
                      logLevelToString(level).c_str(),
                      message.c_str());
        xSemaphoreGive(logMutex);
    }
#else
    if (!shouldLog(level)) return;

    std::lock_guard<std::mutex> guard(logMutex);
    std::ofstream logFile(getLogFileName(), std::ios_base::app);
    if (!logFile) {
        std::cerr << logLevelToString(LogLevel::ERROR) << "Failed to open log file" << std::endl;
        return;
    }

    logFile << "[" << getElapsedTime() << "] "
            << logLevelToString(level) << " "
            << message << std::endl;
#endif
}