#ifndef __PRIME_TESTS_H__
#define __PRIME_TESTS_H__
#include "big_int64.h"
#include "logger.h"

#ifdef SYCL_VERSION

bool isDivisibleBySmallPrimesUSM(sycl::queue &q, const BigInt64 &n);
bool isDivisibleBySmallPrimesBuffers(sycl::queue &q, const BigInt64 &n);

bool millerRabinPrimalityTestUSM(sycl::queue &q, const BigInt64 &number,
                                 size_t k);
bool millerRabinPrimalityTestBuffers(sycl::queue &q, const BigInt64 &number,
                                     size_t k);
#else
#include <atomic>

bool millerRabinPrimalityTest(const BigInt64 &number, size_t k);

bool millerRabinPrimalityTestThreads(const BigInt64 &number, size_t k);

#endif
bool fermatPrimalityTest(const BigInt64 &number, size_t k);

bool isDivisibleBySmallPrimes(const BigInt64 &n);
BigInt64 nextPrime(const BigInt64 &number, size_t k);
BigInt64 nextPrimeDivideToChunks(const BigInt64 &number, size_t k);
#endif  // __PRIME_TESTS_H__
