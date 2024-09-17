// #ifndef __HSM_H__
// #define __HSM_H__
// #include <string>
// #include <map>
// #include <set>
// #include <fstream>
// #include <gmpxx.h>
// #include <vector>
// #include "general.h"

// constexpr size_t RSA_KEY_SIZE = 1024;

// enum KeyPermission
// {
//     VERIFY,
//     SIGN,
//     ENCRYPT,
//     DECRYPT,
//     EXPORTABLE
// };

// struct KeyMetaData
// {
//     std::set<KeyPermission> permissions;
// };

// class Hsm
// {
// public:
//     static encryptionFunction getEncryptionFunctionType(int id);
//     std::string getPublicKeyIdByUserId(int userId, asymmetricFunctions func);
//     std::string getPrivateKeyIdByUserId(int userId, asymmetricFunctions
//     func); int getPrivateKeyId(int userId, encryptionFunction func); static
//     std::string getSymetricKeyId(int userId);
//     // static void addKey(int keyID, AsymmetricKeys *keys, encryptionFunction
//     func, const KeyMetaData& metaData);
//     // static bool hasPermission(int keyID, KeyPermission permission);
//     // static void setKey(int keyId, AsymmetricKeys *keys);
//     // static AsymmetricKeys *getKey(int keyId);
//     // static bool isRsaEncryption(encryptionFunction func);
//     // static void saveKeyWithPermissionToFile(int keyID, const std::string&
//     fileName);
//     // static void generateKey(int userId, KeyPermission permission,
//     std::string (*func)(int)); std::string encryptData(const std::string
//     &key, const std::string &data); void processKeyRequest(int keyId, int
//     userId, KeyPermission permission); bool isPermission(int keyId, int
//     userId, KeyPermission permission); void sendError(const std::string
//     &errorMsg); std::string generateAESKey(int userId,
//     std::vector<KeyPermission> permissions); std::pair<std::string,
//     std::string> generateRSAKeyPair(int userId, std::vector<KeyPermission>
//     permissions); void updateACL(int userId, const std::string &fileName,
//     const std::vector<KeyPermission> &permissions, const std::string &keyId,
//     const std::string &keyType); std::pair<std::string, std::string>
//     generateECCKeyPair(int userId, std::vector<KeyPermission> permissions);
//     std::string generateKeyId();
//     std::string getAuthorizedKey(int userId, std::string &keyId, std::string
//     permission); std::string readFileContent(const std::string &filePath);
//     static Hsm &getInstance();

//     static std::map<int, std::string> RsaPublicKeysIds;
//     static std::map<int, std::string> RsaPrivateKeysIds;
//     static std::map<int, std::string> EccPublicKeysIds;
//     static std::map<int, std::string> EccPrivateKeysIds;
//     static std::map<int, std::string> symmetricKeysIds;

// private:
//     static std::map<int, encryptionFunction> encryptionFunctions;
//     static std::map<int, KeyMetaData> keyMetaData;
//     Hsm() = default;
//     Hsm(const Hsm&) = delete;
//     Hsm& operator=(const Hsm&) = delete;
// };

// #endif // __HSM_H__
