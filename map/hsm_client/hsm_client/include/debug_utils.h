#ifndef  __DEBUG_UTILS_H__
#define  __DEBUG_UTILS_H__

#include <chrono>
#include <iostream>
#include <string>
#include "general.h"

#define START_TIMER                                                            \
  auto start_timer = std::chrono::high_resolution_clock::now();

#define END_TIMER(message)                                                     \
  auto end_timer = std::chrono::high_resolution_clock::now();                  \
  std::chrono::duration<double> elapsed = end_timer - start_timer;             \
  std::cout << message << " took " << elapsed.count() << " seconds\n";

void printBufferHexa(const uint8_t *buffer, size_t len, std::string message);

void encryptStartPrintParams(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen, unsigned char* key, AESKeyLength keyLength);
void encryptContinuePrintParams(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen);

#endif //  __DEBUG_UTILS_H__