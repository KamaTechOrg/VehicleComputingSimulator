#include <string>
#include "../include/crypto_api.h"

StreamAES* AESEcbFactory::create() const  
{
    logger.logMessage(logger::LogLevel::INFO, "using EBC chaining mode");
    return new AESEcb();
};

StreamAES* AESCbcFactory::create() const  
{
    logger.logMessage(logger::LogLevel::INFO, "using CBC chaining mode");
    return new AESCbc();
};

StreamAES* AESCfbFactory::create() const  
{
    logger.logMessage(logger::LogLevel::INFO, "using CFB chaining mode");
    return new AESCfb();
};

StreamAES* AESOfbFactory::create() const  
{
    logger.logMessage(logger::LogLevel::INFO, "using OFB chaining mode");
    return new AESOfb();
};

StreamAES* AESCtrFactory::create() const  
{
    logger.logMessage(logger::LogLevel::INFO, "using CTR chaining mode");
    return new AESCtr();
};
