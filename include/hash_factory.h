#ifndef FACTORY_MANAGER_H
#define FACTORY_MANAGER_H
#include "sha256.h"
#include "SHA3-512.h"
#include "IHash.h"
#include <functional>
#include <map>
#include <memory>
class HashFactory
{
public:
    /**
     * @brief Gets the singleton instance of FactoryManager.
     *
     * @return The singleton instance of FactoryManager.
     */
    static HashFactory& getInstance()
    {
        static HashFactory instance;
        return instance;
    }
    /**
     * @brief Creates an IHash object based on the specified SHAAlgorithm.
     *
     * @param type The SHA algorithm type.
     * @return A unique_ptr to the newly created IHash object.
     */
    std::unique_ptr<IHash> create(const IHash::SHAAlgorithm& type) const
    {
        auto it = factories.find(type);
        if (it != factories.end())
            return it->second();
        return nullptr;
    }
private:
    std::map<IHash::SHAAlgorithm, std::function<std::unique_ptr<IHash>()>> factories =
    {
        {IHash::SHAAlgorithm::SHA256, []() -> std::unique_ptr<IHash> { return std::make_unique<SHA256>(); }},
        {IHash::SHAAlgorithm::SHA3_512, []() -> std::unique_ptr<IHash> { return std::make_unique<SHA3_512>(); }}
    };
    /**
     * @brief Private constructor for singleton pattern.
     */
    HashFactory() {}
};
#endif // FACTORY_MANAGER_H