#ifndef __BIG_INT_UTILS__
#define __BIG_INT_UTILS__

#include <cstdint>
#include <string>
#include <vector>
#define MAX_VAL_64 UINT64_MAX  // 18446744073709551615
#define MAX_VAL_10 9
#define BITS_PER_BYTE 8

inline uint64_t toBase64(uint64_t val)
{
    return MAX_VAL_64 - val + 1;
}

inline void adder3_64(const uint64_t &a, const uint64_t &b,
                      const uint64_t &carryIn, uint64_t &sum,
                      uint64_t &carryOut)
{
    uint64_t ab = a + b;
    // note that if A+B coused overflow, the result will be smaller than both
    uint64_t out = (ab < b) ? 1 : 0;
    sum = ab + carryIn;

    carryOut = out + ((sum < ab) ? 1 : 0);
}

#if defined(__SYCL_DEVICE_ONLY__)  // Check for Intel C++ Compiler
inline void mul2Limbs64(const uint64_t a, const uint64_t b, uint64_t &high,
                        uint64_t &low)
{
    // Break 'a' and 'b' into high and low 32-bit halves
    uint64_t a_low = static_cast<uint32_t>(a);  // Lower 32 bits of 'a'
    uint64_t a_high = a >> 32;                  // Upper 32 bits of 'a'
    uint64_t b_low = static_cast<uint32_t>(b);  // Lower 32 bits of 'b'
    uint64_t b_high = b >> 32;                  // Upper 32 bits of 'b'

    // Perform partial multiplications
    uint64_t low_low = a_low * b_low;      // Lower 32 bits * lower 32 bits
    uint64_t low_high = a_low * b_high;    // Lower 32 bits * upper 32 bits
    uint64_t high_low = a_high * b_low;    // Upper 32 bits * lower 32 bits
    uint64_t high_high = a_high * b_high;  // Upper 32 bits * upper 32 bits

    // Combine the results (handling carry)
    uint64_t mid1 =
        low_high + (low_low >> 32);  // Middle part 1 + carry from low_low
    uint64_t mid2 =
        high_low +
        (mid1 & 0xFFFFFFFF);  // Middle part 2 + lower 32 bits of mid1
    high = high_high + (mid1 >> 32) +
           (mid2 >> 32);  // Sum of upper parts with carry
    low = (mid2 << 32) |
          (low_low & 0xFFFFFFFF);  // Combine lower 32 bits of mid2 and low_low
}
#elif defined(__GNUC__) && defined(__cplusplus)  // Check for g++
inline void mul2Limbs64(const uint64_t a, const uint64_t b, uint64_t &high,
                        uint64_t &low)
{
    __uint128_t result = static_cast<__uint128_t>(a) * b;
    low = static_cast<uint64_t>(result);
    high = static_cast<uint64_t>(result >> 64);
}

#elif defined(_WIN32) || defined(_WIN64)  // Check if running on Windows
#include <intrin.h>                       // Include intrinsics for Windows

inline void mul2Limbs64(const uint64_t a, const uint64_t b, uint64_t &high,
                        uint64_t &low)
{
    low = _umul128(
        a, b,
        &high);  // Perform 64-bit multiplication and get high and low parts
}

#else
#error "Compiler/platform not supported for mul2Limbs64"
#endif

inline uint64_t toBase10(uint8_t val)
{
    // note that 0->0
    return (MAX_VAL_10 - val + 1) % 10;
}
inline void adder3_10(const uint8_t &a, const uint8_t &b,
                      const uint8_t &carryIn, uint8_t &sum, uint8_t &carryOut)
{
    uint8_t abcarry = a + b + carryIn;
    sum = abcarry % 10;
    carryOut = abcarry / 10;
}
inline void mul2Limbs10(const uint8_t a, const uint8_t b, uint8_t &high,
                        uint8_t &low)
{
    uint8_t mult = a * b;
    low = mult % 10;
    high = mult / 10;
}

inline bool isDigit10(const uint8_t a)
{
    return a >= 0 && a <= 9;
}
inline bool isCharDigit10(const uint64_t a)
{
    return a >= '0' && a <= '9';
}
inline int bitsCount(uint64_t a)
{
#if defined(_MSC_VER)  // Check if compiling with Visual Studio

    int count = 0;
    while (a > 0) {
        a >>= 1;
        count++;
    }

    return count;

#else  // Assume Linux or GCC/Clang

    int count = 64 - __builtin_clzll(a);
    return count;
#endif
}

inline uint64_t floatsToUint64(double float1, double float2)
{
    // Scale the floats to 32-bit unsigned integers
    uint32_t part1 =
        static_cast<uint32_t>(float1 * (1ULL << 32));  // Upper 32 bits
    uint32_t part2 =
        static_cast<uint32_t>(float2 * (1ULL << 32));  // Lower 32 bits

    // Combine the two 32-bit parts into a 64-bit unsigned integer
    return (static_cast<uint64_t>(part1) << 32) | static_cast<uint64_t>(part2);
}
#endif  // __BIG_INT_UTILS__