#ifndef __TEMP_HSM_H__
#define __TEMP_HSM_H__

#include "aes.h"
#include "debug_utils.h"
#include "ecc.h"
#include "general.h"
#include "rsa.h"
#include <algorithm>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
constexpr size_t RSA_KEY_SIZE = 1024;

const std::string AES_KEY_ID = "AESKey";
const std::string RSA_PUB_KEY_ID = "RSAPubKey";
const std::string RSA_PRIV_KEY_ID = "RSAPrivKey";
const std::string ECC_PUB_KEY_ID = "ECCPubKey";
const std::string ECC_PRIV_KEY_ID = "ECCPrivKey";

class TempHsm {
   public:
    // Deleted to avoid copying and assignment
    TempHsm(const TempHsm &) = delete;
    TempHsm &operator=(const TempHsm &) = delete;

    // Access the singleton instance
    static TempHsm &getInstance()
    {
        static TempHsm instance;
        return instance;
    }
    // configure a user
    void configure(int userId, CryptoConfig config)
    {
        usersConfig[userId] = CryptoConfig(config);
    }
    // AES key generation for a specific user
    std::string generateAESKey(int ownerId, AESKeyLength aesKeyLength,
                               const std::vector<KeyPermission> &permissions,
                               int destUserId)
    {
        std::string keyId = AES_KEY_ID + std::to_string(ownerId);
        keyIdUsersPermissions[keyId] = {{ownerId, permissions},
                                        {destUserId, {KeyPermission::DECRYPT}}};
        int keySize = aesKeyLengthData[aesKeyLength].keySize;
        std::shared_ptr<unsigned char[]> key =
            std::shared_ptr<unsigned char[]>(new unsigned char[keySize]);
        generateKey(key.get(), aesKeyLength);

        aesKeys[keyId].first = key;
        aesKeys[keyId].second = keySize;

        return keyId;
    }

    // RSA key pair generation for a specific user
    std::pair<std::string, std::string> generateRSAKeyPair(
        int userId, const std::vector<KeyPermission> &permissions)
    {
        std::string pubKeyId = RSA_PUB_KEY_ID + std::to_string(userId);
        std::string privKeyId = RSA_PRIV_KEY_ID + std::to_string(userId);
        // ASK:: does he give other premossions? and premissions to private and
        // public?
        //  Store key pair with permissions
        keyIdUsersPermissions[pubKeyId] = {
            {userId, permissions},
            {-1, {KeyPermission::DECRYPT, KeyPermission::ENCRYPT}}};
        keyIdUsersPermissions[privKeyId] = {{userId, permissions}};
        size_t pubLen = rsaGetPublicKeyLen(RSA_KEY_SIZE);
        size_t priLen = rsaGetPrivateKeyLen(RSA_KEY_SIZE);
        uint8_t *pubBuff = new uint8_t[pubLen];
        uint8_t *priBuff = new uint8_t[priLen];

        CK_RV rv1 =
            rsaGenerateKeys(RSA_KEY_SIZE, pubBuff, pubLen, priBuff, priLen);

        // Assuming keys is a map<string, string>
        rsaKeys[pubKeyId].first = pubBuff;
        rsaKeys[pubKeyId].second = pubLen;
        rsaKeys[privKeyId].first = priBuff;
        rsaKeys[privKeyId].second = priLen;

        return {pubKeyId, privKeyId};
    }

    // ECC key pair generation for a specific user
    std::pair<std::string, std::string> generateECCKeyPair(
        int userId, const std::vector<KeyPermission> &permissions)
    {
        std::string pubKeyId = ECC_PUB_KEY_ID + std::to_string(userId);
        std::string privKeyId = ECC_PRIV_KEY_ID + std::to_string(userId);

        // Store key pair with permissions
        keyIdUsersPermissions[pubKeyId] = {
            {userId, permissions},
            {-1, {KeyPermission::DECRYPT, KeyPermission::ENCRYPT}}};
        keyIdUsersPermissions[privKeyId] = {{userId, permissions}};
        mpz_class privateKey = generatePrivateKey();
        Point publicKey = generatePublicKey(privateKey);

        // Assuming keys is a map<string, string>
        eccPublicKeys[pubKeyId] = publicKey;
        eccPrivateKeys[privKeyId] = privateKey;

        return {pubKeyId, privKeyId};
    }

    // Retrieve public key ID for a user
    std::string getPublicKeyIdByUserId(int userId, AsymmetricFunction function)
    {
        std::string publicKeyId;
        if (function == ECC)
            publicKeyId = ECC_PUB_KEY_ID + std::to_string(userId);
        else
            publicKeyId = RSA_PUB_KEY_ID + std::to_string(userId);
        return publicKeyId;
    }

    // Get private key ID for a user
    std::string getPrivateKeyIdByUserId(int userId, AsymmetricFunction function)
    {
        std::string privateKeyId;
        if (function == ECC)
            privateKeyId = ECC_PRIV_KEY_ID + std::to_string(userId);
        else
            privateKeyId = RSA_PRIV_KEY_ID + std::to_string(userId);
        return privateKeyId;
    }

    std::pair<std::shared_ptr<unsigned char[]>, int> getAESKey(
        int userId, const std::string &keyId, KeyPermission usage)
    {
        checkPermission(keyId, userId, usage);
        // If the usage is DECRYPT, delete it after returning a deep copy
        if (usage == KeyPermission::DECRYPT) {
         //   Get the original key and its length
            std::shared_ptr<unsigned char[]> originalKey(aesKeys[keyId].first);
            int keyLength = aesKeys[keyId].second;

            // Deep copy the key before deleting it
            aesKeys.erase(keyId);  // Remove the key entry from the map

            // Remove permissions associated with the key
            keyIdUsersPermissions[keyId].clear();

            // Return the copied key and its length as a pair
            return {originalKey, keyLength};
        }
        return aesKeys[keyId];
    }

    std::pair<uint8_t *, int> getRSAKey(int userId, const std::string &keyId,
                                        KeyPermission usage)
    {
        checkPermission(keyId, userId, usage);
        return rsaKeys[keyId];
    }

    mpz_class getECCPrivateKey(int userId, const std::string &keyId,
                               KeyPermission usage)
    {
        checkPermission(keyId, userId, usage);
        return eccPrivateKeys[keyId];
    }

    Point getECCPublicKey(int userId, const std::string &keyId,
                          KeyPermission usage)
    {
        checkPermission(keyId, userId, usage);
        return eccPublicKeys[keyId];
    }
    CryptoConfig getUserConfig(int userId)
    {
        return usersConfig[userId];
    }
    AsymmetricFunction getAssymetricFunctionByUserId(int userId)
    {
        return usersConfig[userId].asymmetricFunction;
    }

    // Function to check permissions and throw a basic error if denied
    void checkPermission(const std::string &keyId, int userId,
                         KeyPermission usage)
    {
        // First, check if the permission is for all users (-1)
        if (keyIdUsersPermissions[keyId].count(-1) > 0) {
            const std::vector<KeyPermission> &globalPermissions =
                keyIdUsersPermissions[keyId][-1];
            if (std::find(globalPermissions.begin(), globalPermissions.end(),
                          usage) != globalPermissions.end()) {
                // Permission granted for all users
                return;
            }
        }

        // If not granted for all users, check specific user permissions
        const std::vector<KeyPermission> &permissions =
            keyIdUsersPermissions[keyId][userId];
        if (std::find(permissions.begin(), permissions.end(), usage) ==
            permissions.end()) {
            throw std::runtime_error("Permission denied for keyId: " + keyId +
                                     " and userId: " + std::to_string(userId));
        }
    }

   private:
    TempHsm() {}

    // For each user, store configurations, keys, and permissions
    std::unordered_map<int, CryptoConfig> usersConfig;
    std::unordered_map<std::string, std::map<int, std::vector<KeyPermission>>>
        keyIdUsersPermissions;
    std::unordered_map<std::string,
                       std::pair<std::shared_ptr<unsigned char[]>, int>>
        aesKeys;
    std::unordered_map<std::string, std::pair<uint8_t *, int>> rsaKeys;
    std::unordered_map<std::string, Point> eccPublicKeys;
    std::unordered_map<std::string, mpz_class> eccPrivateKeys;
};
#endif  // __TEMP_HSM_H__