#include "../include/general.h"

void log(logger::LogLevel level, const std::string &message) {
    static logger logInstance("HSM");
    logInstance.logMessage(level, message);
}

bool isValidAESKeyLength(AESKeyLength aesKeyLength)
{    
    // Create a set of valid key lengths and check if the provided key length is in this set
    switch (aesKeyLength) {
        case AES_128:
        case AES_192:
        case AES_256:
            return true;
        default:
            return false;
    }
}