#include <cstdint>
#include <iomanip>

#include <iostream>
#include "debug_utils.h"
using namespace std;

#define DEBUG


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

void encryptStartPrintParams(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen, unsigned char* key, AESKeyLength keyLength)
{
    printBufferHexa(block, inLen, "Block: ");
    printBufferHexa(key, 128, "Key: ");
    std::cout << "inLen: " << inLen << ", outLen(before): " << outLen << std::endl;
}
void encryptContinuePrintParams(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen)
{
    printBufferHexa(block, inLen, "Block: ");
    std::cout << "inLen: " << inLen << ", outLen(before): " << outLen << std::endl;
}

void printStreamAES(const StreamAES& obj, size_t blockSize, std::string message) {
    cout<<message<<endl;

    printBufferHexa(obj.iv, blockSize, "IV: ");

    printBufferHexa(obj.lastBlock, blockSize, "Last Block: ");

    printBufferHexa(obj.key, obj.keyLength, "Key: ");

    //printBufferHexa(obj.lastData, blockSize, "Last Data: ");

    std::cout << "Key Length: " << obj.keyLength << " bytes" << std::endl;
}

// Function to print the EncryptedMessage struct
void printEncryptedMessage(const EncryptedMessage& message) {
    // std::cout << "EncryptedMessage:" << std::endl;
    // std::cout << "c1X: " << message.c1X.get_str() << std::endl;
    // std::cout << "c1Y: " << (message.c1Y ? "true" : "false") << std::endl;
    // std::cout << "c2X: " << message.c2X.get_str() << std::endl;
    // std::cout << "c2Y: " << (message.c2Y ? "true" : "false") << std::endl;
}