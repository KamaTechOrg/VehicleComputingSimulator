#include <cstdint>
#include <iomanip>
#include <iostream>
#include "../include/debug_utils.h"
using namespace std;

#define DEBUG

/**
 * Prints the contents of a buffer in hexadecimal format, along with a message.
 *
 * @param buffer Pointer to the buffer containing data to print.
 * @param len The length of the buffer.
 * @param message A message to display before printing the buffer.
 */
void printBufferHexa(const uint8_t *buffer, size_t len, std::string message)
{
#ifdef DEBUG
    std::cout << message << std::endl;
    for (size_t i = 0; i < len; ++i)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(buffer[i]) << " ";
        // Print a new line every 16 bytes for better readability
        if ((i + 1) % 16 == 0)
            std::cout << std::endl;
    }
    std::cout << std::endl;
    // Reset the stream format back to decimal
    std::cout << std::dec;
#endif
}

/**
 * Prints the parameters related to the encryption start process.
 *
 * @param block Pointer to the input data block to be encrypted.
 * @param inLen The length of the input data block.
 * @param out Reference to a pointer for the output data buffer.
 * @param outLen Reference to the length of the output data buffer.
 * @param key Pointer to the encryption key.
 * @param keyLength The length of the encryption key.
 */
void encryptStartPrintParams(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen, unsigned char* key, AESKeyLength keyLength)
{
    printBufferHexa(block, inLen, "Block: ");
    printBufferHexa(key, 128, "Key: ");
    std::cout << "inLen: " << inLen << ", outLen(before): " << outLen << std::endl;
}

/**
 * Prints the parameters related to the continuation of the encryption process.
 *
 * @param block Pointer to the input data block currently being processed.
 * @param inLen The length of the input data block.
 * @param out Reference to a pointer for the output data buffer.
 * @param outLen Reference to the length of the output data buffer.
 */
void encryptContinuePrintParams(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen)
{
    printBufferHexa(block, inLen, "Block: ");
    std::cout << "inLen: " << inLen << ", outLen(before): " << outLen << std::endl;
}
