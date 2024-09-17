#include <iomanip>
#include <iostream>
#include <map>
#include <cstddef>
#include "crypto_api.h"
#include "hash_factory.h"
#include "rsa.h"
#include "temp_hsm.h"
#include "logger.h"
#include "debug_utils.h"

std::map<int, std::pair<StreamAES *, size_t>> mapToInMiddleEncryptions;
std::map<int, std::pair<StreamAES *, size_t>> mapToInMiddleDecryptions;
std::map<int, std::pair<std::unique_ptr<IHash>, size_t>> mapToInMiddleHashing;

constexpr size_t BITS_IN_BYTE = 8;
constexpr size_t ECC_CIPHER_LENGTH = 512;
constexpr size_t ECC_MAX_DECRYPTED_LENGTH = 256;

std::string keyPermissionToString(KeyPermission permission)
{
    switch (permission) {
        case VERIFY:
            return "VERIFY";
        case SIGN:
            return "SIGN";
        case ENCRYPT:
            return "ENCRYPT";
        case DECRYPT:
            return "DECRYPT";
        case EXPORTABLE:
            return "EXPORTABLE";
        default:
            return "UNKNOWN";
    }
}

std::string permissionsToString(const std::vector<KeyPermission> &permissions)
{
    std::ostringstream oss;
    for (size_t i = 0; i < permissions.size(); ++i) {
        oss << keyPermissionToString(permissions[i]);
        if (i != permissions.size() - 1) {
            oss << ", ";
        }
    }
    return oss.str();
}

/**
 * @brief Boot the system by generating asymmetric keys for users.
 *
 * This function logs the start of the boot process, iterates over a map of user IDs and 
 * their associated key permissions, and generates ECC and RSA key pairs for each user.
 *
 * @param usersIdspermissions A map where each key is a user ID (int), and the value is a 
 *                            vector of KeyPermission objects representing the user's permissions.
 * 
 * @return CKR_OK on successful completion of the key generation process.
 */
CK_RV bootSystem(
    const std::map<int, std::vector<KeyPermission>> &usersIdspermissions)
{
    log(logger::LogLevel::INFO,
        "CryptoApi Boot System: Booting system started...\n Generating "
        "assymetric keys for users: ");
    for (const auto &[userId, permissions] : usersIdspermissions) {
        log(logger::LogLevel::INFO,
            "User ID: " + std::to_string(userId) +
                ", Permissions: " + permissionsToString(permissions));
        TempHsm::getInstance().generateECCKeyPair(userId, permissions);
        TempHsm::getInstance().generateRSAKeyPair(userId, permissions);
    }
    return CKR_OK;
}

/**
 * @brief Adds a user to the HSM by generating asymmetric keys for a user.
 *
 * This function adds a user to the HSM, it generates ECC and RSA 
 * key pairs for the specified user based on their permissions.
 *
 * @param userId The ID of the user that is being added.
 * @param permissions A vector of KeyPermission objects representing the 
 *                    permissions associated with the user.
 * 
 * @return CKR_OK on successful completion of the process.
 */
CK_RV addProccess(int userId, std::vector<KeyPermission> &permissions)
{
    log(logger::LogLevel::INFO,
        "AddProccess: adding proccess...\n Generating "
        "assymetric keys for user: ");
    log(logger::LogLevel::INFO,
        "User ID: " + std::to_string(userId) +
            ", Permissions: " + permissionsToString(permissions));
    TempHsm::getInstance().generateECCKeyPair(userId, permissions);
    TempHsm::getInstance().generateRSAKeyPair(userId, permissions);

    return CKR_OK;
}

CK_RV configure(int userId, CryptoConfig config)
{
    log(logger::LogLevel::INFO,
        "Configure: configuring user: " + std::to_string(userId));
    TempHsm::getInstance().configure(userId, config);
    return CKR_OK;
}

#pragma region RSA and ECC

// Serialize the EncryptedMessage to a void* buffer
void serializeToBuffer(const EncryptedMessage &message, uint8_t *out)
{
    size_t count1, count2;
    std::vector<uint8_t> buffer1, buffer2;
    size_t offset = 0;

    buffer1.resize((mpz_sizeinbase(message.c1X.get_mpz_t(), 2) + 7) /
                   8);  // size in bytes
    mpz_export(buffer1.data(), &count1, 1, 1, 0, 0, message.c1X.get_mpz_t());
    buffer1.resize(count1);  // resize buffer to actual size after export

    buffer2.resize((mpz_sizeinbase(message.c2X.get_mpz_t(), 2) + 7) /
                   8);  // size in bytes
    mpz_export(buffer2.data(), &count2, 1, 1, 0, 0, message.c2X.get_mpz_t());
    buffer2.resize(count2);  // resize buffer to actual size after export

    // Store c1X (length followed by data)
    std::memcpy(out + offset, &count1, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    std::memcpy(out + offset, buffer1.data(), count1);
    offset += count1;

    // Store c1Y
    std::memcpy(out + offset, &message.c1Y, sizeof(message.c1Y));
    offset += sizeof(message.c1Y);

    // Store c2X (length followed by data)
    std::memcpy(out + offset, &count2, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    std::memcpy(out + offset, buffer2.data(), count2);
    offset += count2;

    // Store c2Y
    std::memcpy(out + offset, &message.c2Y, sizeof(message.c2Y));
}

// Deserialize the buffer back to EncryptedMessage
EncryptedMessage deserializeFromBuffer(const void *buffer, size_t bufferSize)
{
    size_t offset = 0;
    const uint8_t *byteBuffer = static_cast<const uint8_t *>(buffer);

    // Deserialize c1X
    mpz_class c1X;
    size_t count1 = byteBuffer[offset];
    offset += sizeof(uint8_t);
    mpz_import(c1X.get_mpz_t(), count1, 1, 1, 0, 0, byteBuffer + offset);
    offset += count1;

    // Deserialize c1Y
    bool c1Y;
    std::memcpy(&c1Y, byteBuffer + offset, sizeof(c1Y));
    offset += sizeof(c1Y);

    // Deserialize c2X
    mpz_class c2X;
    size_t count2 = byteBuffer[offset];
    offset += sizeof(uint8_t);
    mpz_import(c2X.get_mpz_t(), count2, 1, 1, 0, 0, byteBuffer + offset);
    offset += count2;

    // Deserialize c2Y
    bool c2Y;
    std::memcpy(&c2Y, byteBuffer + offset, sizeof(c2Y));

    return EncryptedMessage(c1X, c1Y, c2X, c2Y);
}

size_t getEncryptedLengthByAssymFunc(AsymmetricFunction func)
{
    if (func == RSA)
        return getRSAencryptedLength();
    else
        return getECCencryptedLength();
}

size_t getRSAencryptedLength()
{
    return rsaGetEncryptedLen(RSA_KEY_SIZE);
}

size_t getRSAdecryptedLength()
{
    return rsaGetDecryptedLen(RSA_KEY_SIZE);
}

size_t getECCencryptedLength()
{
    return 2 * (sizeof(uint8_t) + sizeof(bool)) +
           ECC_CIPHER_LENGTH / BITS_IN_BYTE;
}

size_t getECCdecryptedLength()
{
    return ECC_MAX_DECRYPTED_LENGTH / BITS_IN_BYTE;
}

std::string getPublicECCKeyByUserId(int userId)
{
    return TempHsm::getInstance().getPublicKeyIdByUserId(
        userId, AsymmetricFunction::ECC);
}

std::string getPublicRSAKeyByUserId(int userId)
{
    return TempHsm::getInstance().getPublicKeyIdByUserId(
        userId, AsymmetricFunction::RSA);
}

std::string getPrivateECCKeyByUserId(int userId)
{
    return TempHsm::getInstance().getPrivateKeyIdByUserId(
        userId, AsymmetricFunction::ECC);
}

std::string getPrivateRSAKeyByUserId(int userId)
{
    return TempHsm::getInstance().getPrivateKeyIdByUserId(
        userId, AsymmetricFunction ::RSA);
}

/**
 * @brief Encrypts data using Elliptic Curve Cryptography (ECC).
 *
 * This function performs ECC encryption on the input data using the specified sender ID and key ID.
 * It retrieves the corresponding ECC public key from the HSM and encrypts the input data using the public key.
 * The encrypted data is then serialized and stored in the output buffer.
 *
 * @param senderId The ID of the sender.
 * @param keyId The ID of the ECC public key to be used for encryption.
 * @param in The input data to be encrypted.
 * @param inLen The length of the input data.
 * @param out The buffer where the encrypted output will be stored.
 * @param outLen The length of the encrypted output.
 * @return CKR_OK on success or an appropriate error code on failure.
 */
CK_RV ECCencrypt(int senderId, std::string keyId, void *in, size_t inLen,
                 void *out, size_t &outLen)
{
    log(logger::LogLevel::INFO,
        "Starting ECC encryption for user id: " + std::to_string(senderId) +
            " with keyId: " + keyId);
    std::vector<uint8_t> inVec(static_cast<uint8_t *>(in),
                               static_cast<uint8_t *>(in) + inLen);
    Point eccPublicKey;
    try {
        eccPublicKey = TempHsm::getInstance().getECCPublicKey(
            senderId, keyId, KeyPermission::ENCRYPT);
    }
    catch (std::exception &e) {
        log(logger::LogLevel::ERROR,
            "Failed to retrieve ECC public key for user id: " +
                std::to_string(senderId) + ", keyId: " + keyId +
                ". Error: " + e.what());
        return CKR_USER_NOT_AUTHORIZED;
    }
    //perform ecc encryption
    EncryptedMessage cipher = encryptECC(inVec, eccPublicKey);

    //cast cipher from EncryptedMessage to out buffer
    serializeToBuffer(cipher, static_cast<uint8_t *>(out));

    log(logger::LogLevel::INFO,
        "Successfully completed ECC encryption for user id: " +
            std::to_string(senderId));

    return CKR_OK;
}

/**
 * @brief Decrypts data using Elliptic Curve Cryptography (ECC).
 *
 * This function decrypts the input data using the ECC decryption algorithm and the provided key.
 * It retrieves the private ecc key from the HSM for the specified user and key ID, then decrypts the input data.
 *
 * @param receiverId The ID of the reciever.
 * @param keyId The ID of the key to be used for decryption.
 * @param in A pointer to the input data to be decrypted.
 * @param inLen The length of the input data.
 * @param out A pointer to the buffer where the decrypted output will be stored.
 * @param[out] outLen The length of the decrypted output.
 *
 * @return CKR_OK if the decryption is successful, or an appropriate error code if the decryption fails.
 *
 * @note This function logs the start and end of the decryption process for debugging purposes.
 * @note If the decryption fails due to an unauthorized key, it returns CKR_USER_NOT_AUTHORIZED.
 */
CK_RV ECCdecrypt(int receiverId, std::string keyId, void *in, size_t inLen,
                 void *out, size_t &outLen)
{
    // std::string eccPrivateKeyString;
    // try {
    //   eccPrivateKeyString =
    //       TempHsm::getInstance().getAuthorizedKey(receiverId, keyId,
    //       "DECRYPT");
    // } catch (std::exception &e) {
    //   return CKR_USER_NOT_AUTHORIZED;
    // }
    // mpz_class eccPrivateKey(eccPrivateKeyString);
    // // Decrypt the message
    // std::vector<uint8_t> decryptedMessage =
    //     decryptECC(*(reinterpret_cast<EncryptedMessage *>(in)),
    //     eccPrivateKey);
    // outLen = decryptedMessage.size();
    // std::memcpy(out, decryptedMessage.data(), outLen);

    // return CKR_OK;
    log(logger::LogLevel::INFO,
        "Starting ECC decryption for user id: " + std::to_string(receiverId) +
            " with keyId: " + keyId);
    mpz_class key;
    try {
        key = TempHsm::getInstance().getECCPrivateKey(receiverId, keyId,
                                                      KeyPermission::DECRYPT);
    }
    catch (std::exception &e) {
        log(logger::LogLevel::ERROR,
            "Failed to retrieve ECC private key for user id: " +
                std::to_string(receiverId) + ", keyId: " + keyId +
                ". Error: " + e.what());
        return CKR_USER_NOT_AUTHORIZED;
    }

    //cast the cipher from in buffer to EncryptedMessage
    EncryptedMessage cipher = deserializeFromBuffer(in, inLen);
    
    //perform decryption
    std::vector<uint8_t> decryptedMessage = decryptECC(cipher, key);
    outLen = decryptedMessage.size();
    std::memcpy(out, decryptedMessage.data(), outLen);

    log(logger::LogLevel::INFO,
        "Successfully completed ECC decryption for user id: " +
            std::to_string(receiverId));

    return CKR_OK;
}

/**
 * @brief Encrypts data using RSA.
 *
 * This function performs RSA encryption on the input data using the specified sender ID and key ID.
 * It retrieves the corresponding RSA key from the HSM and encrypts the input data.
 * The encrypted data is then stored in the output buffer.
 *
 * @param userId The ID of the user.
 * @param keyId The ID of the RSA encryption key to be used.
 * @param in The input data to be encrypted.
 * @param inLen The length of the input data.
 * @param out The buffer where the encrypted output will be stored.
 * @param outLen The length of the buffer for the encrypted output.
 *
 * @return CKR_OK on successful encryption, or CKR_USER_NOT_AUTHORIZED if the key retrieval fails.
 */
CK_RV RSAencrypt(int userId, std::string keyId, void *in, size_t inLen,
                 void *out, size_t outLen)
{
    // std::string rsaKeyString;
    // try {
    //   rsaKeyString =
    //       TempHsm::getInstance().getAuthorizedKey(userId, keyId, "ENCRYPT");
    // } catch (std::exception &e) {
    //   return CKR_USER_NOT_AUTHORIZED;
    // }
    // CK_RV returnCode =
    //     rsaEncrypt(reinterpret_cast<uint8_t *>(in), inLen,
    //                reinterpret_cast<const uint8_t *>(rsaKeyString.c_str()),
    //                rsaKeyString.size(), reinterpret_cast<uint8_t *>(out),
    //                outLen, rsaKeyString.size() * BITS_IN_BYTE);
    log(logger::LogLevel::INFO,
        "Starting RSA encryption for user id: " + std::to_string(userId) +
            " with keyId: " + keyId);
    std::pair<uint8_t *, int> key;
    try {
        key = TempHsm::getInstance().getRSAKey(userId, keyId,
                                               KeyPermission::ENCRYPT);
    }
    catch (std::exception &e) {
        log(logger::LogLevel::ERROR,
            "Failed to retrieve RSA key for user id: " +
                std::to_string(userId) + ", keyId: " + keyId +
                ". Error: " + e.what());
        return CKR_USER_NOT_AUTHORIZED;
    }
    CK_RV returnCode = rsaEncrypt(
        reinterpret_cast<uint8_t *>(in), inLen, key.first, key.second,
        reinterpret_cast<uint8_t *>(out), outLen, RSA_KEY_SIZE);
    log(logger::LogLevel::INFO,
        "Successfully completed RSA encryption for user id: " +
            std::to_string(userId));
    return returnCode;
}

/**
 * @brief Decrypts data using RSA.
 *
 * This function decrypts the input data using the RSA decryption algorithm and the provided key.
 * It retrieves the corresponding RSA key from the HSM for the specified user and key ID, then decrypts the input data.
 * The decrypted data is then stored in the output buffer.
 *
 * @param userId The ID of the user.
 * @param keyId The ID of the RSA decryption key to be used.
 * @param in The input data to be decrypted.
 * @param inLen The length of the input data.
 * @param out The buffer where the decrypted output will be stored.
 * @param outLen The length of the buffer for the decrypted output.
 *
 * @return CKR_OK on successful decryption, or CKR_USER_NOT_AUTHORIZED if the
 * key retrieval fails, or the return code fron rsa decryption.
 */
CK_RV RSAdecrypt(int userId, std::string keyId, void *in, size_t inLen,
                 void *out, size_t *outLen)
{
    // std::string rsaKeyString;
    // try {
    //   rsaKeyString =
    //       TempHsm::getInstance().getAuthorizedKey(userId, keyId, "DECRYPT");
    // } catch (std::exception &e) {
    //   return CKR_USER_NOT_AUTHORIZED;
    // }
    // CK_RV returnCode =
    //     rsaDecrypt(reinterpret_cast<uint8_t *>(in), inLen,
    //                reinterpret_cast<const uint8_t *>(rsaKeyString.c_str()),
    //                rsaKeyString.size(), reinterpret_cast<uint8_t *>(out),
    //                outLen, rsaKeyString.size() * BITS_IN_BYTE);
    // return returnCode;
    log(logger::LogLevel::INFO,
        "Starting RSA decryption for user id: " + std::to_string(userId) +
            " with keyId: " + keyId);
    std::pair<uint8_t *, int> key;
    try {
        key = TempHsm::getInstance().getRSAKey(userId, keyId,
                                               KeyPermission::DECRYPT);
    }
    catch (std::exception &e) {
        log(logger::LogLevel::ERROR,
            "Failed to retrieve RSA key for user id: " +
                std::to_string(userId) + ", keyId: " + keyId +
                ". Error: " + e.what());
        return CKR_USER_NOT_AUTHORIZED;
    }
    CK_RV returnCode = rsaDecrypt(
        reinterpret_cast<uint8_t *>(in), inLen, key.first, key.second,
        reinterpret_cast<uint8_t *>(out), outLen, RSA_KEY_SIZE);

    log(logger::LogLevel::INFO,
        "Successfully completed RSA decryption for user id: " +
            std::to_string(userId));
    return returnCode;
}

// Generates a pair of asymmetric ECC keys and returns their keyIds
std::pair<std::string, std::string> generateECCKeyPair(
    int userId, std::vector<KeyPermission> permissions)
{
    //todo logging
    return TempHsm::getInstance().generateECCKeyPair(userId, permissions);
}

// Generates a pair of asymmetric RSA keys and returns their keyIds
std::pair<std::string, std::string> generateRSAKeyPair(
    int userId, std::vector<KeyPermission> permissions)
{
    //todo logging
    return TempHsm::getInstance().generateRSAKeyPair(userId, permissions);
}

#pragma endregion RSA and ECC

#pragma region AES

// Retrieves an AES key from the HSM by key ID.
void retrieveAESKeyByKeyId(int userId, std::string aesKeyId,
                           std::shared_ptr<unsigned char[]> symmetricKey,
                           size_t symmetricKeyLen, KeyPermission permission)
{
    std::pair<std::shared_ptr<unsigned char[]>, int> keyAndLength =
        TempHsm::getInstance().getAESKey(userId, aesKeyId, permission);
    symmetricKey = keyAndLength.first;
}

//function to encrypt symmetric key with RSA or ECC
CK_RV encryptAESkey(int senderId, int recieverId, uint8_t *symmetricKey,
                    size_t symmetricKeyLen, void *encryptedKey,
                    size_t encryptedKeyLen, AsymmetricFunction func)
{
    std::string recieversPublicKeyId =
        TempHsm::getInstance().getPublicKeyIdByUserId(recieverId, func);
    CK_RV returnCode;

    // encrypt symmetric key with ECC or RSA with recievers public key
    if (func == RSA)
        returnCode = RSAencrypt(senderId, recieversPublicKeyId, symmetricKey,
                                symmetricKeyLen, encryptedKey, encryptedKeyLen);
    else {
        returnCode = ECCencrypt(senderId, recieversPublicKeyId, symmetricKey,
                                symmetricKeyLen, encryptedKey, encryptedKeyLen);
    }
    // printBufferHex(encryptedKey, encryptedKeyLen, "encrypted key");

    return returnCode;
}

//function to decrypt symmetric key with RSA or ECC
CK_RV decryptAESkey(int senderId, int recieverId, void *in, size_t inLen,
                    uint8_t *symmetricKey, size_t &symmetricKeyLen,
                    AsymmetricFunction func)
{
    std::string recieverrsPrivateKeyId =
        TempHsm::getInstance().getPrivateKeyIdByUserId(recieverId, func);
    CK_RV returnCode;
    // decrypt symmetric key with ECC or RSA with recievers private key
    if (func == RSA)
        returnCode = RSAdecrypt(recieverId, recieverrsPrivateKeyId, in, inLen,
                                symmetricKey, &symmetricKeyLen);
    else
        returnCode = ECCdecrypt(recieverId, recieverrsPrivateKeyId, in, inLen,
                                symmetricKey, symmetricKeyLen);

    printBufferHexa(symmetricKey, symmetricKeyLen, "decrypted key");

    return returnCode;
}

//Performs AES encryption on the first data block.
CK_RV performAESEncryption(int senderId, void *in, size_t inLen, void *out,
                           size_t outLen, AESChainingMode chainingMode,
                           std::shared_ptr<unsigned char[]> symmetricKey,
                           AESKeyLength keyLength, size_t counter)
{
    StreamAES *streamAES = FactoryManager::getInstance().create(chainingMode);
    mapToInMiddleEncryptions[senderId] = std::make_pair(streamAES, counter);

    mapToInMiddleEncryptions[senderId].first->encryptStart(
        reinterpret_cast<unsigned char *>(in), inLen,
        static_cast<unsigned char *>(out), outLen, symmetricKey.get(),
        keyLength);

    return CKR_OK;
}

// Encrypts data using AES encryption with optional key generation or retrieval.
CK_RV AESencrypt(int senderId, int receiverId, void *in, size_t inLen,
                 void *out, size_t outLen, AESKeyLength keyLength,
                 AESChainingMode chainingMode, size_t counter,
                 std::string keyId, bool generateKeyFlag,
                 AsymmetricFunction func)
{
    CK_RV returnCode;
    size_t encryptedKeyLength = 0;

    //if first chunck
    if (mapToInMiddleEncryptions.count(senderId) == 0) {
        size_t symmetricKeyLen = keyLength;
        std::shared_ptr<unsigned char[]> key;
        // Handle key generation or retrieval:
        // if using key generation - generate a new key and concatenate it to the encrypted data
        if (generateKeyFlag) {
            key =
                std::shared_ptr<unsigned char[]>(new unsigned char[keyLength]);
            generateKey(key.get(), keyLength);
            encryptedKeyLength = getEncryptedLengthByAssymFunc(func);
            //encrypt the symmetric key and store it in the out buffer
            returnCode = encryptAESkey(
                senderId, receiverId, key.get(), symmetricKeyLen, out,
                getEncryptedLengthByAssymFunc(func), func);

            if (returnCode != CKR_OK)
                return returnCode;
        }
        //otherwise retrieve the key from file by keyId
        else {
            // retrieveAESKeyByKeyId(senderId, keyId, key,
            //                       symmetricKeyLen, ENCRYPT);
            key = TempHsm::getInstance()
                      .getAESKey(senderId, keyId, ENCRYPT)
                      .first;
        }
        //printBufferHexa(symmetricKey, symmetricKeyLen, "key retrieved for encrypting");
        log(logger::LogLevel::INFO, "Performing AES encryption for user id: " +
                                        std::to_string(senderId) +
                                        " for chunck of data number 1 with keyId: " + keyId +" .");
        // Perform AES encryption
        returnCode = performAESEncryption(
            senderId, in, inLen,
            static_cast<uint8_t *>(out) + encryptedKeyLength,
            outLen - encryptedKeyLength, chainingMode, key, keyLength, counter);
        if (returnCode != CKR_OK)
            return returnCode;
    }
    else {
        // Handle chunk continuation
        log(logger::LogLevel::INFO,
            "Performing AES encryption for user id: " +
                std::to_string(senderId) + " for chunck of data number " +
                std::to_string(counter -
                               mapToInMiddleEncryptions[senderId].second + 1) +
                ".");
        //perform encryption
        mapToInMiddleEncryptions[senderId].first->encryptContinue(
            reinterpret_cast<unsigned char *>(in), inLen,
            static_cast<unsigned char *>(out), outLen);
    }

    //reduce a chunck from the chuncks counter
    mapToInMiddleEncryptions[senderId].second--;

    // If all chunks have been encrypted, erase the entry from the map
    if (mapToInMiddleEncryptions[senderId].second == 0) {
        auto it = mapToInMiddleEncryptions.find(senderId);
        mapToInMiddleEncryptions.erase(senderId);
        log(logger::LogLevel::INFO,
        "Successfully completed AES encryption for user id: " +
            std::to_string(senderId)+" for all "+std::to_string(counter)+" chuncks.");
    }

    return CKR_OK;
}

// Performs AES decryption on the first data block.
void performAESDecryption(int recieverId, void *in, size_t inLen, void *out,
                          size_t &outLen, AESChainingMode chainingMode,
                          std::shared_ptr<unsigned char[]> symmetricKey,
                          AESKeyLength keyLength, size_t counter,
                          bool generateKeyFlag)
{
    StreamAES *streamAES = FactoryManager::getInstance().create(chainingMode);
    mapToInMiddleDecryptions[recieverId] = std::make_pair(streamAES, counter);
    unsigned int outLen2 = outLen;

    mapToInMiddleDecryptions[recieverId].first->decryptStart(
        reinterpret_cast<unsigned char *>(in), inLen,
        static_cast<unsigned char *>(out), outLen2, symmetricKey.get(),
        keyLength);
    
    outLen = outLen2;
}

// Decrypts data using AES decryption with optional key generation or retrieval.
CK_RV AESdecrypt(int senderId, int receiverId, void *in, size_t inLen,
                 void *out, size_t &outLen, AsymmetricFunction func,
                 AESKeyLength keyLength, AESChainingMode chainingMode,
                 size_t counter, bool generateKeyFlag, std::string keyId = "")
{
    if (mapToInMiddleDecryptions.count(receiverId) == 0) {
        std::shared_ptr<unsigned char[]> symmetricKey;
        size_t offset = 0;
        // Handle key generation or retrieval:
        //if using key generation - decrypt the concatenated key
        if (generateKeyFlag) {
            symmetricKey =
                std::shared_ptr<unsigned char[]>(new unsigned char[keyLength]);
            size_t encryptedKeyLength =
                getEncryptedLengthByAssymFunc(func);
            size_t symmetricKeyLength = keyLength;
            //decrypt the symmetric key
            CK_RV returnCode =
                decryptAESkey(senderId, receiverId, in, encryptedKeyLength,
                              symmetricKey.get(), symmetricKeyLength, func);
            offset = encryptedKeyLength;

            if (returnCode != CKR_OK)
                return returnCode;
        }
        //otherwise retrieve the key from file by keyId
        else {
            // retrieveAESKeyByKeyId(receiverId, keyId, symmetricKey,
            //                       symmetricKeyLen, DECRYPT);
            symmetricKey = TempHsm::getInstance()
                               .getAESKey(receiverId, keyId, DECRYPT)
                               .first;
        }
        log(logger::LogLevel::INFO, "Performing AES decryption for user id: " +
                                        std::to_string(receiverId) +
                                        " for chunck of data number 1 with keyId: " + keyId +" .");
        // Perform AES decryption
        performAESDecryption(receiverId,
                             static_cast<unsigned char *>(in) + offset,
                             inLen - offset, out, outLen, chainingMode,
                             symmetricKey, keyLength, counter, generateKeyFlag);
    }
    else {
        // Handle chunk continuation
        log(logger::LogLevel::INFO,
            "Performing AES decryption for user id: " +
                std::to_string(receiverId) + " for chunck of data number " +
                std::to_string(
                    counter - mapToInMiddleDecryptions[receiverId].second + 1) +
                ".");
        unsigned int outLen2 = outLen;
        mapToInMiddleDecryptions[receiverId].first->decryptContinue(
            reinterpret_cast<unsigned char *>(in), inLen,
            static_cast<unsigned char *>(out), outLen2);
        outLen = outLen2;
    }
    // reduce a chunck from the chuncks counter
    mapToInMiddleDecryptions[receiverId].second--;

    // If all chunks have been decrypted, erase the entry from the map
    if (mapToInMiddleDecryptions[receiverId].second == 0) {
        auto it = mapToInMiddleDecryptions.find(receiverId);
        mapToInMiddleDecryptions.erase(receiverId);
        log(logger::LogLevel::INFO,
        "Successfully completed AES decryption for user id: " +
            std::to_string(receiverId)+" for all "+std::to_string(counter)+" chuncks.");
    }

    return CKR_OK;
}

// Generates a symmetric AES key, writes it to file and returns its keyId
std::string generateAESKey(int userId, AESKeyLength aesKeyLength,
                           std::vector<KeyPermission> permissions,
                           int destUserId)
{
    //todo logging
    return TempHsm::getInstance().generateAESKey(userId, aesKeyLength,
                                                 permissions, destUserId);
}

// Function to calculate length of encrypted data by AES when using a keyId.
size_t getAESencryptedLength(size_t dataLen, bool isFirst,
                             AESChainingMode chainingMode)
{
    return calculatEncryptedLenAES(dataLen, isFirst, chainingMode);
}

// Function to calculate length of decrypted data by AES  when using a keyId.
size_t getAESdecryptedLength(size_t dataLen, bool isFirst,
                             AESChainingMode chainingMode)
{
    return calculatDecryptedLenAES(dataLen, isFirst, chainingMode);
}

/**
 * @brief Encrypts data using AES.
 * 
 * This function performs AES encryption.
 * It performs encryption using the provided sender and receiver IDs, input data, and key ID. 
 * Unlike the inner `AESencrypt` function, this version does not generate a new AES key 
 * but instead retrieves an existing key.
 * 
 * @param senderId The ID of the sender.
 * @param receiverId The ID of the receiver.
 * @param in The input data to be encrypted.
 * @param inLen The length of the input data.
 * @param[out] out The buffer where the encrypted output will be stored.
 * @param[out] outLen The length of the encrypted output.
 * @param func The asymmetric encryption function used (e.g., RSA, ECC).
 * @param keyLength The AES key length (128, 192, or 256 bits).
 * @param chainingMode The AES chaining mode (e.g., CBC, CTR).
 * @param counter The counter for the chunked encryption process.
 * @param keyId The ID of the key to be retrieved.
 * @return CKR_OK on success or an appropriate error code on failure.
 */
CK_RV AESencrypt(int senderId, int receiverId, void *in, size_t inLen,
                 void *out, size_t outLen, AESKeyLength keyLength,
                 AESChainingMode chainingMode, size_t counter,
                 std::string keyId)
{
    return AESencrypt(senderId, receiverId, in, inLen, out, outLen, keyLength,
                      chainingMode, counter, keyId, false, RSA);
}

/**
 * @brief Decrypts data using AES.
 * 
 * This function performs AES decryption. 
 * It performs decryption using the provided sender and receiver IDs, input data, and key ID. 
 * Unlike the inner `AESdecrypt` function, this version does not get an aes key concatenated 
 * to data but instead retrieves an existing key.
 * 
 * @param senderId The ID of the sender.
 * @param receiverId The ID of the receiver.
 * @param in The input data to be decrypted.
 * @param inLen The length of the input data.
 * @param[out] out The buffer where the decrypted output will be stored.
 * @param[out] outLen The length of the decrypted output.
 * @param keyLength The AES key length (128, 192, or 256 bits).
 * @param chainingMode The AES chaining mode (e.g., CBC, CTR).
 * @param counter The chuncks counter for the chunked decryption process.
 * @param keyId The ID of the key to be retrieved.
 * @return CKR_OK on successful encryption, an appropriate error code on failure..
 */
CK_RV AESdecrypt(int senderId, int receiverId, void *in, size_t inLen,
                 void *out, size_t &outLen, AESKeyLength keyLength,
                 AESChainingMode chainingMode, size_t counter,
                 std::string keyId)
{
    return AESdecrypt(senderId, receiverId, in, inLen, out, outLen, RSA,
                      keyLength, chainingMode, counter, false, keyId);
}

#pragma endregion RSA and ECC

#pragma region SIGN VERIFY

size_t getHashedMessageSize(SHAAlgorithm hashFunc)
{
    switch (hashFunc) {
        case SHAAlgorithm::SHA_256:
            return 256 / BITS_IN_BYTE;
        case SHAAlgorithm::SHA_3_512:
            return 512 / BITS_IN_BYTE;
        default:
            throw std::invalid_argument("Invalid hash function");
    }
}

bool isDoneHashing(int userId)
{
    return mapToInMiddleHashing.count(userId) != 0 &&
           mapToInMiddleHashing[userId].second == 0;
}

bool isFirstTimeHash(int userId)
{
    return mapToInMiddleHashing.count(userId) == 0;
}

// Updates the hash with the provided data.
CK_RV hashDataUpdate(int userId, void *data, size_t dataLen,
                     SHAAlgorithm hashfunc, int counter)
{
    CK_RV returnCode = CKR_OK;
    if (isFirstTimeHash(userId)) {  // first time
        HashFactory *factoryManager = &HashFactory::getInstance();
        mapToInMiddleHashing[userId] =
            std::make_pair(std::unique_ptr<IHash>(), counter);
        returnCode = factoryManager->create(hashfunc,
                                            mapToInMiddleHashing[userId].first);
    }
    std::vector<uint8_t>(static_cast<uint8_t *>(data),
                         static_cast<uint8_t *>(data) + dataLen);
    mapToInMiddleHashing[userId].first->update(std::vector<uint8_t>(
        static_cast<uint8_t *>(data), static_cast<uint8_t *>(data) + dataLen));
    mapToInMiddleHashing[userId].second--;
    return returnCode;
}

// Finalizes the hash computation and retrieves the resulting hash.
CK_RV hashDataFinalize(int userId, void *out, size_t outLen)
{
    std::vector<uint8_t> result;
    CK_RV returnCode = mapToInMiddleHashing[userId].first->finalize(result);
    auto it = mapToInMiddleHashing.find(userId);
    mapToInMiddleHashing.erase(userId);
    memcpy(out, result.data(), outLen);
    return returnCode;
}

size_t getSignatureLength()
{
    return getRSAencryptedLength();
}

// Updates the hash with the provided data for signing.
CK_RV signUpdate(int senderId, void *data, size_t dataLen,
                 SHAAlgorithm hashfunc, int counter)
{
    log(logger::LogLevel::INFO,
        "Signing for user id: " + std::to_string(senderId) +
            " chunck of data.");
    return hashDataUpdate(senderId, data, dataLen, hashfunc, counter);
}

// Finalizes the signing process and retrieves the resulting digital signature.
CK_RV signFinalize(int senderId, void *signature, size_t signatureLen,
                   SHAAlgorithm hashfunc, std::string keyId)
{
    log(logger::LogLevel::INFO,
        "Signing for user id: " + std::to_string(senderId) +
            " for last chunck of data.");
    size_t hashLen = getHashedMessageSize(hashfunc);
    std::vector<uint8_t> hash(hashLen);
    CK_RV returnCode = hashDataFinalize(senderId, hash.data(), hashLen);
    if (returnCode != CKR_OK)
        return returnCode;
    returnCode = RSAencrypt(senderId, keyId, hash.data(), hashLen, signature,
                            signatureLen);
    return returnCode;
}

// Verifies and updates the hash with the provided data.
CK_RV verifyUpdate(int recieverId, void *data, size_t dataLen,
                   SHAAlgorithm hashFunc, size_t counter)
{
    log(logger::LogLevel::INFO,
        "Verifying for user id: " + std::to_string(recieverId) +
            " chunck of data.");
    return hashDataUpdate(recieverId, data, dataLen, hashFunc, counter);
}

// Verifies the digital signature of the hashed input data and finalizes the signature verification process.
CK_RV verifyFinalize(int recieverId, void *signature, size_t signatureLen,
                     SHAAlgorithm hashFunc, std::string keyId)
{
    log(logger::LogLevel::INFO,
        "Verifying for user id: " + std::to_string(recieverId) +
            " for last chunck of data.");
    size_t hashLen = getHashedMessageSize(hashFunc);
    std::vector<uint8_t> hash(hashLen);
    CK_RV returnCode = hashDataFinalize(recieverId, hash.data(), hashLen);
    if (returnCode != CKR_OK)
        return returnCode;
    size_t decryptSignatureLen = rsaGetDecryptedLen(RSA_KEY_SIZE);
    std::vector<uint8_t> decryptSignature(decryptSignatureLen);
    returnCode = RSAdecrypt(recieverId, keyId, signature, signatureLen,
                            decryptSignature.data(), &decryptSignatureLen);
    if (returnCode != CKR_OK)
        return returnCode;
    if (decryptSignatureLen != hashLen ||
        memcmp(decryptSignature.data(), hash.data(), decryptSignatureLen) !=
            0) {
        returnCode = CKR_SIGNATURE_INVALID;
        log(logger::LogLevel::ERROR,
            "Verifying signature failed for user id: " +
                std::to_string(recieverId) + ".");
    }
    return returnCode;
}

#pragma endregion SIGN VERIFY

#pragma region ENCRYPT DECRYPT

/**
 * @brief Calculates the length of the encrypted data.
 *
 * This function calculates the length of the encrypted data based on the given input length,
 * whether it is the first chunk of data, and the sender's encryption configuration.
 *
 * @param senderId The ID of the sender, used to retrieve the encryption configuration.
 * @param inLen The length of the input data.
 * @param isFirst A boolean indicating whether it is the first chunk of data.
 *
 * @return The length of the encrypted data.
 *
 * @note The length includes the encrypted padded data, plus the encrypted symmetric key
 *       if it is the first chunk of data.
 */
size_t getEncryptedLen(int senderId, size_t inLen, bool isFirst)
{
    // Retrieve the encryption function type for the given sender ID
    CryptoConfig config = TempHsm::getInstance().getUserConfig(senderId);
    // encrypted padded data (+ if first chunck: encrypted symmetric key)
    return getAESencryptedLength(inLen, isFirst, config.aesChainingMode) +
           (isFirst ? getEncryptedLengthByAssymFunc(config.asymmetricFunction) : 0);
}

/**
 * @brief Calculates the length of the decrypted data.
 *
 * This function calculates the length of the decrypted data based on the given input length,
 * whether it is the first chunk of data, and the sender's encryption configuration.
 *
 * @param senderId The ID of the sender, used to retrieve the encryption configuration.
 * @param inLen The length of the input data.
 * @param isFirst A boolean indicating whether it is the first chunk of data.
 *
 * @return The length of the decrypted data.
 *
 * @note The length calculation includes the decrypted padded data.
 */
size_t getDecryptedLen(int senderId, size_t inLen, bool isFirst)
{
    // Retrieve the encryption function type for the given sender ID
    CryptoConfig config = TempHsm::getInstance().getUserConfig(senderId);
    size_t encryptedLength =
        (inLen - (isFirst ? getEncryptedLengthByAssymFunc(config.asymmetricFunction)
                          : 0));

    return calculatDecryptedLenAES(encryptedLength, isFirst,
                                   config.aesChainingMode);
}


/**
 * @brief Encrypts the input data using AES and signs it with RSA.
 *
 * This function performs the encryption and signing operations for the input data.
 * It uses the sender's user ID to retrieve its encryption and signing configurations.
 * The encrypted data is then signed using the sender's private key.
 * and by the last chunck the signature is sent in the signature buffer.
 * The encryption and signing are based on the settings defined in the sender's user configuration.
 *
 * @param senderId The ID of the sender, used to retrieve the encryption and signing configurations.
 * @param receiverId The ID of the receiver, used to retrieve the public key for encryption.
 * @param in The input data to be encrypted and signed.
 * @param inLen The length of the input data.
 * @param out The buffer to store the encrypteddata.
 * @param outLen The length of the encrypted data.
 * @param signature The buffer to store the digital signature.
 * @param signatureLen The length of the digital signature.
 * @param counter The counter value used for streaming.
 *
 * @return CKR_OK on success, or an appropriate error code on failure.
 */
CK_RV encrypt(int senderId, int receiverId, void *in, size_t inLen, void *out,
              size_t outLen, void *signature, size_t signatureLen,
              size_t counter)
{
    CK_RV returnCode;
    CryptoConfig config = TempHsm::getInstance().getUserConfig(senderId);

    std::string recieversPublicKeyId =
        TempHsm::getInstance().getPublicKeyIdByUserId(
            receiverId, config.asymmetricFunction);

    //perform encryption
    returnCode =
        AESencrypt(senderId, receiverId, in, inLen, out, outLen,
                   config.aesKeyLength, config.aesChainingMode, counter,
                   recieversPublicKeyId, true, config.asymmetricFunction);
    if (returnCode != CKR_OK)
        return returnCode;

    //sign the data
    returnCode = signUpdate(senderId, in, inLen, config.hashFunction, counter);
    if (returnCode != CKR_OK)
        return returnCode;
    if (isDoneHashing(senderId)) {
        std::string senderPrivateKeyId =
            TempHsm::getInstance().getPrivateKeyIdByUserId(senderId, RSA);
        returnCode = signFinalize(senderId, signature, signatureLen,
                                  config.hashFunction, senderPrivateKeyId);
    }

    if (returnCode != CKR_OK)
        return returnCode;
    return CKR_OK;
}

/**
 * @brief  Decrypts the input data using AES and verifies it with RSA.
 *
 * This function gets the digital signature and encrypted data,
 * decrypts the data using AES, and verifies the digital signature using RSA. 
 * The decryption and signature verification are based on the settings defined in the sender's user configuration.
 *
 * @param senderId The ID of the sender, used for retrieving the signature verification settings.
 * @param receiverId The ID of the receiver, used for retrieving the private key for decryption.
 * @param in The input buffer containing the concatenated signature and encrypted data.
 * @param inLen The length of the input buffer.
 * @param out The buffer to store the decrypted data.
 * @param[out] outLen The length of the decrypted data.
 * @param counter The counter value used for streaming.
 *
 * @return CKR_OK on success, or an appropriate error code on failure.
 */
CK_RV decrypt(int senderId, int receiverId, void *in, size_t inLen,
              void *signature, size_t signatureLen, void *out, size_t &outLen,
              size_t counter)
{
    CK_RV returnCode;
    CryptoConfig config = TempHsm::getInstance().getUserConfig(senderId);
    
    //perform decryption
    returnCode = AESdecrypt(senderId, receiverId, in, inLen, out, outLen,
                            config.asymmetricFunction, config.aesKeyLength,
                            config.aesChainingMode, counter, true);
    if (returnCode != CKR_OK)
        return returnCode;

    //perform signature verification
    returnCode =
        verifyUpdate(receiverId, out, outLen, config.hashFunction, counter);
    if (returnCode != CKR_OK)
        return returnCode;
    if (isDoneHashing(receiverId)) {
        std::string senderPublicKeyId =
            TempHsm::getInstance().getPublicKeyIdByUserId(senderId, RSA);
        returnCode = verifyFinalize(receiverId, signature, signatureLen,
                                    config.hashFunction, senderPublicKeyId);
    }
    
    if (returnCode != CKR_OK)
        return returnCode;
    return CKR_OK;
}

#pragma endregion ENCRYPT DECRYPT

//////////////////////////////////////
//json copying to maps
// /**
//  * @brief Loads key data from a JSON file and populates key maps.
//  *
//  * This function reads a JSON file containing key information and populates
//  * the corresponding key maps with user IDs and key IDs. It handles
//  different
//  * key types, including ECC and RSA private and public keys. The JSON file
//  * should have a structure where each key entry includes the key type, user
//  ID,
//  * and key ID.
//  *
//  * @note The JSON file is expected to be located at "../keys/keys.json".
//  *       The key types supported are "ECC-Private", "ECC-Public",
//  "RSA-Private",
//  *       and "RSA-Public".
//  *
//  * @return void This function does not return any value.
//  *
//  * @throws std::ifstream::failure If the file could not be opened for
//  reading.
//  */
// void loadDataFromJson() {
//   std::string filePath = "../keys/keys.json";
//   std::ifstream file(filePath);
//   if (!file.is_open()) {
//     std::cerr << "Could not open the file!" << std::endl;
//     return;
//   }
//   nlohmann::json jsonData;
//   file >> jsonData;
//   for (const auto &key : jsonData["keys"]) {
//     // Check if the key_type is "ECC-Private"
//     int user;
//     std::string keyId;
//     if (key["key_type"] == "ECC-Private") {
//       // Extract user and key_id
//       user = key["user"];
//       keyId = key["key_id"];
//       // Insert into the map
//       TempHsm::EccPrivateKeysIds[user] = keyId;
//     } else if (key["key_type"] == "ECC-Public") {
//       user = key["user"];
//       keyId = key["key_id"];
//       TempHsm::EccPublicKeysIds[user] = keyId;
//     } else if (key["key_type"] == "RSA-Private") {
//       user = key["user"];
//       keyId = key["key_id"];
//       TempHsm::RsaPrivateKeysIds[user] = keyId;
//     } else if (key["key_type"] == "RSA-Public") {
//       user = key["user"];
//       keyId = key["key_id"];
//       TempHsm::RsaPublicKeysIds[user] = keyId;
//     }
//   }
//   // Close the file after reading
//   file.close();
// }