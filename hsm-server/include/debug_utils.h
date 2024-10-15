#ifndef __DEBUG_UTILS_H__
#define __DEBUG_UTILS_H__

#include <chrono>
#include <iostream>
#include <string>
#include "general.h"

#define START_TIMER \
    auto start_timer = std::chrono::high_resolution_clock::now();

#define END_TIMER(message)                                           \
    auto end_timer = std::chrono::high_resolution_clock::now();      \
    std::chrono::duration<double> elapsed = end_timer - start_timer; \
    std::cout << message << " took " << elapsed.count() << " seconds\n";

void printBufferHexa(const uint8_t *buffer, size_t len, std::string message);

void debugLog(const std::string &message,
              const std::string &functionName);  // Macro for easier use
#define DEBUG_LOG(msg) debugLog(msg, __func__)
#define LOG_BUFFER_HEXA(buffer, len, message) \
    logBufferHexa(buffer, len, message, __func__, __LINE__)
void logBufferHexa(const void *voidBuffer, size_t len,
                   const std::string &message, const char *callingFunction,
                   int line);

#endif  //  __DEBUG_UTILS_H__
