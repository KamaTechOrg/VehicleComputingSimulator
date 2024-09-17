#ifndef STREAM_AES_FACTORY_H
#define STREAM_AES_FACTORY_H

#include "aes.h"
#include "aes_stream.h"
#include <map>
#include <string>

/**
 * @brief Singleton class for managing StreamAESFactory instances.
 */
class FactoryManager 
{
   public:
    /**
     * @brief Gets the singleton instance of FactoryManager.
     * 
     * @return The singleton instance of FactoryManager.
     */
    static FactoryManager& getInstance() ;
    // {
    //     return instance;
    // }

    /**
     * @brief Creates a StreamAES object based on the specified AESChainingMode.
     * 
     * @param type The AES chaining mode.
     * @return A pointer to the newly created StreamAES object.
     */
    StreamAES* create(const AESChainingMode& type) const; 
    // {
    //     auto it = factories.find(type);
    //     if (it != factories.end()) 
    //         return it->second;
        
    //     return nullptr;
    // }
        static FactoryManager instance;

   private:
    std::map<AESChainingMode, StreamAES*> factories = 
    {
      {AESChainingMode::ECB, new AESEcb()},
      {AESChainingMode::CBC, new AESCbc()},
      {AESChainingMode::CFB, new AESCfb()},
      {AESChainingMode::OFB, new AESOfb()},
      {AESChainingMode::CTR, new AESCtr()}
    };

    /**
     * @brief Private constructor for singleton pattern.
     */
    FactoryManager() {}
};
#endif