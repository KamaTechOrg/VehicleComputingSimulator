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

void RSAdecryptPrintParams(int userId, std::string keyId, void *in, size_t inLen, void *out, size_t *outLen) {
    // Print the userId
    std::cout << "User ID: " << userId << std::endl;
    
    // Print the keyId
    std::cout << "Key ID: " << keyId << std::endl;

    // Print the input data in hex format
    std::cout << "Input Data: ";
    unsigned char* inputData = static_cast<unsigned char*>(in);
    for (size_t i = 0; i < inLen; ++i) {
        std::cout << std::hex << static_cast<int>(inputData[i]) << " ";
    }
    std::cout << std::endl;

    // Print the input length
    std::cout << "Input Length: " << inLen << std::endl;

    // Assuming output is also a byte array, we can print it similarly
    if (*outLen > 0) {
        std::cout << "Output Data: ";
        unsigned char* outputData = static_cast<unsigned char*>(out);
        for (size_t i = 0; i < *outLen; ++i) {
            std::cout << std::hex << static_cast<int>(outputData[i]) << " ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "Output Data: (No data yet)" << std::endl;
    }

    // Print the output length
    std::cout << "Output Length: " << *outLen << std::endl;
}

void printGlobalMaps(std::map<int, std::pair<StreamAES *, size_t>> mapToInMiddleEncryptions, 
std::map<int, std::pair<StreamAES *, size_t>> mapToInMiddleDecryptions, 
std::map<int, std::pair<std::unique_ptr<IHash>, size_t>> mapToInMiddleHashing)
{
    std::cout << "Contents of mapToInMiddleEncryptions:" << std::endl;
    for (const auto &entry : mapToInMiddleEncryptions)
    {
        StreamAES *stream = entry.second.first;
        size_t size = entry.second.second;

        std::cout << "StreamAES pointer: " << stream << ", Size: " << size << std::endl;
    }

    std::cout << "Contents of mapToInMiddleDecryptions:" << std::endl;
    for (const auto &entry : mapToInMiddleDecryptions)
    {
        StreamAES *stream = entry.second.first;
        size_t size = entry.second.second;

        std::cout <<"userId: "<< std::to_string(entry.first)  << "StreamAES pointer: " << stream << ", Size: " << size << std::endl;
    }

    std::cout << "Contents of mapToInMiddleHashing:" << std::endl;
    for (const auto &entry : mapToInMiddleHashing)
    {
        IHash *hash = entry.second.first.get();  // Since this is a unique_ptr, we use .get() to access the raw pointer.
        size_t size = entry.second.second;

        std::cout <<"userId: "<< std::to_string(entry.first) << " IHash pointer: " << hash << ", Size: " << size << std::endl;
    }
}