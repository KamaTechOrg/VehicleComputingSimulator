
#include "../include/aes_stream_factory.h"
#include "../include/crypto_api.h"

FactoryManager FactoryManager::instance;

FactoryManager &FactoryManager::getInstance() 
{ 
    return instance; 
}

std::string aesChainingModeToString(AESChainingMode mode) 
{
    switch (mode) {
        case AESChainingMode::ECB: return "ECB";
        case AESChainingMode::CBC: return "CBC";
        case AESChainingMode::CFB: return "CFB";
        case AESChainingMode::OFB: return "OFB";
        case AESChainingMode::CTR: return "CTR";
        default: return "Unknown";
    }
}

StreamAES* FactoryManager::create(const AESChainingMode &type) const 
{
    std::string modeString = aesChainingModeToString(type);
    logger.logMessage(logger::LogLevel::INFO, "using " + modeString + " chaining mode");

    auto it = factories.find(type);
    if (it != factories.end())
        return it->second;

    return nullptr;
}