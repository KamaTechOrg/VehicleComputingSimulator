#ifndef __TESTS_UTILS_H__
#define __TESTS_UTILS_H__
#include <chrono>
#include <iostream>
#include <string>
#ifndef NDEBUG
#define START_TIMER                                                            \
  auto start_timer = std::chrono::high_resolution_clock::now();

#define END_TIMER(message)                                                     \
  auto end_timer = std::chrono::high_resolution_clock::now();                  \
  std::chrono::duration<double> elapsed = end_timer - start_timer;             \
  std::cout << message << " took " << elapsed.count() << " seconds\n";
#else
// Empty macros for Release build
#define START_TIMER
#define END_TIMER(message)
#endif

void printBufferHexa(const unsigned char *buffer, size_t len,
                     const std::string &message);

#endif // __TESTS_UTILS_H__
