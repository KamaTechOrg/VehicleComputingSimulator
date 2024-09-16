// #ifndef __HSM_H__
// #define __HSM_H__

// #include <string>
// #include <map>
// #include <gmpxx.h>
// #include "asymmetric_keys.h"

// #define rsaKeySize 1024

// enum encryptionFunction {
//     // SHA3-512 Signature Modes
//     SIGNATURE_SHA3_512_CBC,
//     SIGNATURE_SHA3_512_CFB,
//     SIGNATURE_SHA3_512_CTR,
//     SIGNATURE_SHA3_512_ECB,
//     SIGNATURE_SHA3_512_OFB,

//     // SHA3-512 Encryption and Sign Modes with 128-bit encryption
//     ENCRYPT_128_AND_SIGN_SHA3_512_CBC,
//     ENCRYPT_128_AND_SIGN_SHA3_512_CFB,
//     ENCRYPT_128_AND_SIGN_SHA3_512_CTR,
//     ENCRYPT_128_AND_SIGN_SHA3_512_ECB,
//     ENCRYPT_128_AND_SIGN_SHA3_512_OFB,

//     // SHA3-512 Encryption and Sign Modes with 192-bit encryption
//     ENCRYPT_192_AND_SIGN_SHA3_512_CBC,
//     ENCRYPT_192_AND_SIGN_SHA3_512_CFB,
//     ENCRYPT_192_AND_SIGN_SHA3_512_CTR,
//     ENCRYPT_192_AND_SIGN_SHA3_512_ECB,
//     ENCRYPT_192_AND_SIGN_SHA3_512_OFB,

//     // SHA3-512 Encryption and Sign Modes with 256-bit encryption
//     ENCRYPT_256_AND_SIGN_SHA3_512_CBC,
//     ENCRYPT_256_AND_SIGN_SHA3_512_CFB,
//     ENCRYPT_256_AND_SIGN_SHA3_512_CTR,
//     ENCRYPT_256_AND_SIGN_SHA3_512_ECB,
//     ENCRYPT_256_AND_SIGN_SHA3_512_OFB,

//     // SHA-256 Signature Modes
//     SIGNATURE_SHA256_CBC,
//     SIGNATURE_SHA256_CFB,
//     SIGNATURE_SHA256_CTR,
//     SIGNATURE_SHA256_ECB,
//     SIGNATURE_SHA256_OFB,

//     // SHA-256 Encryption and Sign Modes with 128-bit encryption
//     ENCRYPT_128_AND_SIGN_SHA256_CBC,
//     ENCRYPT_128_AND_SIGN_SHA256_CFB,
//     ENCRYPT_128_AND_SIGN_SHA256_CTR,
//     ENCRYPT_128_AND_SIGN_SHA256_ECB,
//     ENCRYPT_128_AND_SIGN_SHA256_OFB,

//     // SHA-256 Encryption and Sign Modes with 192-bit encryption
//     ENCRYPT_192_AND_SIGN_SHA256_CBC,
//     ENCRYPT_192_AND_SIGN_SHA256_CFB,
//     ENCRYPT_192_AND_SIGN_SHA256_CTR,
//     ENCRYPT_192_AND_SIGN_SHA256_ECB,
//     ENCRYPT_192_AND_SIGN_SHA256_OFB,

//     // SHA-256 Encryption and Sign Modes with 256-bit encryption
//     ENCRYPT_256_AND_SIGN_SHA256_CBC,
//     ENCRYPT_256_AND_SIGN_SHA256_CFB,
//     ENCRYPT_256_AND_SIGN_SHA256_CTR,
//     ENCRYPT_256_AND_SIGN_SHA256_ECB,
//     ENCRYPT_256_AND_SIGN_SHA256_OFB
// };

// enum hashFunction{
//     SHA_256,
//     SHA3_512
// };

// class Hsm {
//    public:
//     static encryptionFunction getEncryptionFunctionType(int id);
//     static void addProccessKeyandFuncType(int keyID, encryptionFunction func);
//     static void setKey(int keyId, AsymmetricKeys *keys);
//     static AsymmetricKeys *getKey(int keyId);
//     static bool isRsaEncryption(encryptionFunction func);

//     // File management
//     static void saveKeyToFile(int keyId, const std::string &filename);
//     static void loadKeyFromFile(int keyId, const std::string &filename);

//     // Permissions management
//     static void setPermissions(int keyId, const std::string &permissionsFile);
//     static std::string getPermissions(int keyId);

//    private:
//     static std::map<int, AsymmetricKeys *> keys;
//     static std::map<int, encryptionFunction> encryptionFunctions;
//     static std::map<int, std::string> permissions; // Store permissions for each key
// };

// #endif  // __HSM_H__
#ifndef __HSM_H__
#define __HSM_H__

#include <string>
#include <map>
#include <set>
#include <fstream>
#include <gmpxx.h>
#include "asymmetric_keys.h"

#define rsaKeySize 1024

enum encryptionFunction {
    // SHA3-512 Signature Modes
    SIGNATURE_SHA3_512_CBC,
    SIGNATURE_SHA3_512_CFB,
    SIGNATURE_SHA3_512_CTR,
    SIGNATURE_SHA3_512_ECB,
    SIGNATURE_SHA3_512_OFB,

    // SHA3-512 Encryption and Sign Modes with 128-bit encryption
    ENCRYPT_128_AND_SIGN_SHA3_512_CBC,
    ENCRYPT_128_AND_SIGN_SHA3_512_CFB,
    ENCRYPT_128_AND_SIGN_SHA3_512_CTR,
    ENCRYPT_128_AND_SIGN_SHA3_512_ECB,
    ENCRYPT_128_AND_SIGN_SHA3_512_OFB,

    // SHA3-512 Encryption and Sign Modes with 192-bit encryption
    ENCRYPT_192_AND_SIGN_SHA3_512_CBC,
    ENCRYPT_192_AND_SIGN_SHA3_512_CFB,
    ENCRYPT_192_AND_SIGN_SHA3_512_CTR,
    ENCRYPT_192_AND_SIGN_SHA3_512_ECB,
    ENCRYPT_192_AND_SIGN_SHA3_512_OFB,

    // SHA3-512 Encryption and Sign Modes with 256-bit encryption
    ENCRYPT_256_AND_SIGN_SHA3_512_CBC,
    ENCRYPT_256_AND_SIGN_SHA3_512_CFB,
    ENCRYPT_256_AND_SIGN_SHA3_512_CTR,
    ENCRYPT_256_AND_SIGN_SHA3_512_ECB,
    ENCRYPT_256_AND_SIGN_SHA3_512_OFB,

    // SHA-256 Signature Modes
    SIGNATURE_SHA256_CBC,
    SIGNATURE_SHA256_CFB,
    SIGNATURE_SHA256_CTR,
    SIGNATURE_SHA256_ECB,
    SIGNATURE_SHA256_OFB,

    // SHA-256 Encryption and Sign Modes with 128-bit encryption
    ENCRYPT_128_AND_SIGN_SHA256_CBC,
    ENCRYPT_128_AND_SIGN_SHA256_CFB,
    ENCRYPT_128_AND_SIGN_SHA256_CTR,
    ENCRYPT_128_AND_SIGN_SHA256_ECB,
    ENCRYPT_128_AND_SIGN_SHA256_OFB,

    // SHA-256 Encryption and Sign Modes with 192-bit encryption
    ENCRYPT_192_AND_SIGN_SHA256_CBC,
    ENCRYPT_192_AND_SIGN_SHA256_CFB,
    ENCRYPT_192_AND_SIGN_SHA256_CTR,
    ENCRYPT_192_AND_SIGN_SHA256_ECB,
    ENCRYPT_192_AND_SIGN_SHA256_OFB,

    // SHA-256 Encryption and Sign Modes with 256-bit encryption
    ENCRYPT_256_AND_SIGN_SHA256_CFB,
    ENCRYPT_256_AND_SIGN_SHA256_CTR,
    ENCRYPT_256_AND_SIGN_SHA256_ECB,
    ENCRYPT_256_AND_SIGN_SHA256_OFB
};

enum KeyPermission {
    VERIFY, 
    SIGN, 
    ENCRYPT,
    DECRYPT,
    EXPORTABLE
};

struct KeyMetaData {
    std::set<KeyPermission> permissions;
};

class Hsm {
   public:
    // static encryptionFunction getEncryptionFunctionType(int id);
    // static void addKey(int keyID, AsymmetricKeys *keys, encryptionFunction func, const KeyMetaData& metaData);
    // static bool hasPermission(int keyID, KeyPermission permission);
    // static void setKey(int keyId, AsymmetricKeys *keys);
    // static AsymmetricKeys *getKey(int keyId);
    // static bool isRsaEncryption(encryptionFunction func);
    // static void saveKeyWithPermissionToFile(int keyID, const std::string& fileName);
    // static void generateKey(int userId, KeyPermission permission, std::string (*func)(int));
    std::string encryptData(const std::string& key, const std::string& data);
    void processKeyRequest(int keyId, int userId,KeyPermission permission);
    bool isPermission(int keyId, int userId, KeyPermission permission);
    void sendError(const std::string& errorMsg);
    std::string generateAESKey(int userId, std::vector<KeyPermission> permissions);
    // std::pair<std::string, std::string> generateRSAKeyPair(int userId, std::vector<KeyPermission> permissions);
    void updateACL(int userId, const std::string& fileName, const std::vector<KeyPermission>& permissions, const std::string& keyId, const std::string& keyType);
    std::pair<std::string, std::string> generateECCKeyPair(int userId, std::vector<KeyPermission> permissions);
    std::string generateKeyId();
    std::string getAuthorizedKey(int userId,std::string& keyId, std::string& permission);
    std::string readFileContent(const std::string& filePath);
   private:
    static std::map<int, AsymmetricKeys *> keys;
    static std::map<int, encryptionFunction> encryptionFunctions;
    static std::map<int, KeyMetaData> keyMetaData;
};

#endif  // __HSM_H__
