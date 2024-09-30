#include <iomanip>
#include <iostream>
#include <map>
#include <cstddef>
#include "../include/crypto_api.h"
#include "../include/hash_factory.h"
#include "../include/rsa.h"
#include "../include/temp_hsm.h"
#include "../logger/logger.h"
#include "../include/debug_utils.h"

std::map<int, std::pair<StreamAES *, size_t>> mapToInMiddleEncryptions;
std::map<int, std::pair<StreamAES *, size_t>> mapToInMiddleDecryptions;
std::map<int, std::pair<std::unique_ptr<IHash>, size_t>> mapToInMiddleHashing;

constexpr size_t BITS_IN_BYTE = 8;
constexpr size_t ECC_CIPHER_LENGTH = 512;
constexpr size_t ECC_MAX_DECRYPTED_LENGTH = 256;

/**
 * Converts a KeyPermission enumeration value to its corresponding string representation.
 *
 * @param permission The KeyPermission value to convert.
 * @return A string representation of the KeyPermission.
 */
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

/**
 * Converts a vector of KeyPermission values to a comma-separated string representation.
 *
 * @param permissions A vector of KeyPermission values to convert.
 * @return A string representation of the KeyPermissions, separated by commas.
 */
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

/**
 * Configures the user with the provided CryptoConfig settings.
 *
 * @param userId The ID of the user to configure.
 * @param config The CryptoConfig settings to apply to the user.
 * @return CKR_OK on successful configuration.
 */
CK_RV configure(int userId, CryptoConfig config)
{
    log(logger::LogLevel::INFO,
        "Configure: configuring user: " + std::to_string(userId));
    TempHsm::getInstance().configure(userId, config);
    return CKR_OK;
}

#pragma region RSA and ECC

/**
 * Returns the encrypted length based on the specified asymmetric function.
 *
 * @param func The asymmetric function (RSA or ECC) to use for length calculation.
 * @return The encrypted length in bytes for the specified function.
 */
size_t getEncryptedLengthByAssymFunc(AsymmetricFunction func)
{
    if (func == RSA)
        return getRSAencryptedLength();
    else
        return getECCencryptedLength();
}

/**
 * Returns the encrypted length for RSA encryption.
 *
 * @return The RSA encrypted length in bytes.
 */
size_t getRSAencryptedLength()
{
    return rsaGetEncryptedLen(RSA_KEY_SIZE);
}

/**
 * Returns the decrypted length for RSA encryption.
 *
 * @return The RSA decrypted length in bytes.
 */
size_t getRSAdecryptedLength()
{
    return rsaGetDecryptedLen(RSA_KEY_SIZE);
}

/**
 * Returns the encrypted length for ECC encryption.
 *
 * @return The ECC encrypted length in bytes.
 */
size_t getECCencryptedLength()
{
    return 2 * (sizeof(uint8_t) + sizeof(bool)) +
           ECC_CIPHER_LENGTH / BITS_IN_BYTE;
}

/**
 * Returns the decrypted length for ECC encryption.
 *
 * @return The ECC decrypted length in bytes.
 */
size_t getECCdecryptedLength()
{
    return ECC_MAX_DECRYPTED_LENGTH / BITS_IN_BYTE;
}

/**
 * Retrieves the public ECC key for the specified user ID.
 *
 * @param userId The ID of the user whose public ECC key is to be retrieved.
 * @return The public ECC key as a string.
 */
std::string getPublicECCKeyByUserId(int userId)
{
    return TempHsm::getInstance().getPublicKeyIdByUserId(
        userId, AsymmetricFunction::ECC);
}

/**
 * Retrieves the public RSA key for the specified user ID.
 *
 * @param userId The ID of the user whose public RSA key is to be retrieved.
 * @return The public RSA key as a string.
 */
std::string getPublicRSAKeyByUserId(int userId)
{
    return TempHsm::getInstance().getPublicKeyIdByUserId(
        userId, AsymmetricFunction::RSA);
}


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
    return TempHsm::getInstance().generateECCKeyPair(userId, permissions);
}

// Generates a pair of asymmetric RSA keys and returns their keyIds
std::pair<std::string, std::string> generateRSAKeyPair(
    int userId, std::vector<KeyPermission> permissions)
{
    return TempHsm::getInstance().generateRSAKeyPair(userId, permissions);
}

#pragma endregion RSA and ECC


#pragma region AES
//////////////AES//////////////////////////////////////////////////////////////

// //Generates a symmetric AES key,write it to file and returns its keyId
std::string generateAESKey(int userId, AESKeyLength aesKeyLength,
                           std::vector<KeyPermission> permissions,
                           int destUserId)
{
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
 * @brief Retrieves an AES key from the HSM by key ID.
 * 
 * This function retrieves the AES key from the HSM using the provided sender ID
 * and key ID, allocating memory for the key and setting its length.
 * 
 * @param senderId The ID of the sender requesting the key.
 * @param aesKeyId The ID of the AES key to be retrieved.
 * @param[out] symmetricKey A pointer to the retrieved AES key.
 * @param[out] symmetricKeyLen The length of the retrieved AES key in bytes.
 */
void retrieveAESKeyByKeyId(int userId, std::string aesKeyId,
                           std::shared_ptr<unsigned char[]> symmetricKey,
                           size_t symmetricKeyLen, KeyPermission permission)
{
    std::pair<std::shared_ptr<unsigned char[]>, int> keyAndLength =
        TempHsm::getInstance().getAESKey(userId, aesKeyId, permission);
    symmetricKey = keyAndLength.first;
}

//function to encrypt symmetric key with RSA or ECC
CK_RV decryptAESkey(int senderId, int recieverId, void *in, size_t inLen,
                    uint8_t *symmetricKey, size_t &symmetricKeyLen,
                    AsymmetricFunction func)
{
    std::string recieverrsPrivateKeyId =
        TempHsm::getInstance().getPrivateKeyIdByUserId(recieverId, func);
    CK_RV returnCode;
    // decrypt with RSA symmetric key with recievers private key
    if (func == RSA) {
        returnCode = RSAdecrypt(recieverId, recieverrsPrivateKeyId, in, inLen,
                                symmetricKey, &symmetricKeyLen);
    }
    else
        // decrypt with ECC symmetric key with recievers private key
        returnCode = ECCdecrypt(recieverId, recieverrsPrivateKeyId, in, inLen,
                                symmetricKey, symmetricKeyLen);

    return returnCode;
}

/**
 * @brief Performs AES decryption on the first data block.
 * 
 * This function performs AES decryption on the provided input data using the
 * specified chaining mode and symmetric key. The result is written to the output
 * buffer, and a chunk counter is initialized for multi-part decryption.
 * 
 * @param senderId The ID of the sender.
 * @param in The input data to be decrypted.
 * @param inLen The length of the input data.
 * @param[out] out The buffer where the decrypted output will be stored.
 * @param[out] outLen The length of the decrypted output.
 * @param chainingMode The AES chaining mode (e.g., CBC, CTR).
 * @param symmetricKey The symmetric AES key used for encryption.
 * @param symmetricKeyLen The length of the symmetric AES key.
 * @param counter The counter for the chunked decryption process.
 * @return CKR_OK on success or an appropriate error code on failure.
 */
CK_RV performAESDecryption(int recieverId, void *in, size_t inLen, void *out,
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
        reinterpret_cast<unsigned char *&>(out), outLen2, symmetricKey.get(),
        keyLength);
    
    outLen = outLen2;
}

/**
 * @brief Handles the AES decryption of subsequent chunks of data.
 * 
 * This function continues AES decryption on a subsequent chunk of data using the
 * pre-existing decryption context. It also updates the chunk counter and removes
 * the receiver from the decryption map once all chunks are processed.
 * 
 * @param recieverId The ID of the receiver.
 * @param in The input data chunk to be decrypted.
 * @param inLen The length of the input data chunk.
 * @param[out] out The buffer where the decrypted output will be stored.
 * @param[out] outLen The length of the decrypted output.
 */
void AEShandleDataChunksDecryption(int recieverId, void *in, size_t inLen,
                                   void *out, size_t &outLen,
                                   AESChainingMode chainingMode)
{
    unsigned int outLen2=outLen;
    mapToInMiddleDecryptions[recieverId].first->decryptContinue(
        reinterpret_cast<unsigned char *>(in), inLen,
        reinterpret_cast<unsigned char *&>(out), outLen2);
        outLen=outLen2;
}

/**
 * @brief Decrypts data using AES decryption with optional key generation or retrieval.
 * 
 * This function performs AES decryption, either by generating a temporary AES key or
 * retrieving one by key ID. It also handles the continuation of chunked data decryption.
 * 
 * @param senderId The ID of the sender.
 * @param receiverId The ID of the receiver.
 * @param in The input data to be decrypted.
 * @param inLen The length of the input data.
 * @param[out] out The buffer where the decrypted output will be stored.
 * @param[out] outLen The length of the decrypted output.
 * @param func The asymmetric decryption function used (e.g., RSA, ECC).
 * @param keyLength The AES key length (128, 192, or 256 bits).
 * @param chainingMode The AES chaining mode (e.g., CBC, CTR).
 * @param counter The counter for the chunked encryption process.
 * @param keyId The ID of the key (used when retrieving an existing key).
 * @param generateKeyFlag Indicates whether to generate a new AES key (true) or retrieve an existing one (false).
 * @return CKR_OK on success or an appropriate error code on failure.
 */
CK_RV AESdecrypt(int senderId, int receiverId, void *in, size_t inLen,
                 void *out, size_t &outLen, AsymmetricFunction func,
                 AESKeyLength keyLength, AESChainingMode chainingMode,
                 size_t counter, bool generateKeyFlag, std::string keyId = "")
{
    CK_RV returnCode = CKR_OK;
    if (mapToInMiddleDecryptions.count(receiverId) == 0) {
        std::shared_ptr<unsigned char[]> symmetricKey;
        size_t offset = 0;
        // Handle key generation or retrieval
        if (generateKeyFlag) {
            symmetricKey =std::shared_ptr<unsigned char[]>(new unsigned char[keyLength]);
            size_t encryptedKeyLength =
                getEncryptedLengthByAssymFunc(func);
                size_t symmetricKeyLength = keyLength;
            returnCode =
                decryptAESkey(senderId, receiverId, in, encryptedKeyLength,
                              symmetricKey.get(), symmetricKeyLength, func);
            // memset(symmetricKey.get(), 0, keyLength);
            offset = encryptedKeyLength;
            if (returnCode != CKR_OK) {
                return returnCode;
            }
        }
        else {
            symmetricKey = TempHsm::getInstance()
                               .getAESKey(receiverId, keyId, DECRYPT)
                               .first;
        }

        returnCode=performAESDecryption(
            receiverId, static_cast<unsigned char *>(in) + offset,
            inLen - offset, out, outLen, chainingMode, symmetricKey, keyLength,
            counter, generateKeyFlag);

        if (returnCode != CKR_OK)
            return returnCode;
    }
    else {
        // Handle chunk continuation
        AEShandleDataChunksDecryption(receiverId, in, inLen, out, outLen,
                                      chainingMode);
    }
    // reduce a chunck from the chuncks counter
    mapToInMiddleDecryptions[receiverId].second--;

    if (mapToInMiddleDecryptions[receiverId].second == 0) {
        auto it = mapToInMiddleDecryptions.find(receiverId);

        mapToInMiddleDecryptions.erase(receiverId);
    }

    return CKR_OK;
}

/**
 * @brief User-facing function to handle AES decryption using an existing AES key.
 * 
 * This function is the primary function that the user calls to perform AES decryption. 
 * It performs decryption using the provided sender and receiver IDs, input data, and key ID. 
 * Unlike the inner `AESdecrypt` function, this version does not generate a new AES key 
 * but instead retrieves an existing key.
 * 
 * @param senderId The ID of the sender.
 * @param receiverId The ID of the receiver.
 * @param in The input data to be decrypted.
 * @param inLen The length of the input data.
 * @param[out] out The buffer where the decrypted output will be stored.
 * @param[out] outLen The length of the decrypted output.
 * @param func The asymmetric decryption function used (e.g., RSA, ECC).
 * @param keyLength The AES key length (128, 192, or 256 bits).
 * @param chainingMode The AES chaining mode (e.g., CBC, CTR).
 * @param counter The counter for the chunked decryption process.
 * @param keyId The ID of the key to be retrieved.
 * @return CKR_OK on success or an appropriate error code on failure.
 */
CK_RV AESdecrypt(int senderId, int receiverId, void *in, size_t inLen,
                 void *out, size_t &outLen, AESKeyLength keyLength,
                 AESChainingMode chainingMode, size_t counter,
                 std::string keyId)
{
    return AESdecrypt(senderId, receiverId, in, inLen, out, outLen, RSA,
                      keyLength, chainingMode, counter, false, keyId);
}

//function to encrypt symmetric key with RSA or ECC,the encrypted is in out
CK_RV encryptAESkey(int senderId, int recieverId, uint8_t *symmetricKey,
                    size_t symmetricKeyLen, void *encryptedKey,
                    size_t encryptedKeyLen, AsymmetricFunction func)
{
    std::string recieverrsPublicKeyId =
        TempHsm::getInstance().getPublicKeyIdByUserId(recieverId, func);
    CK_RV returnCode;
    // encrypt wuth RSA symmetric key with recievers public key
    if (func == RSA)
        returnCode = RSAencrypt(senderId, recieverrsPublicKeyId, symmetricKey,
                                symmetricKeyLen, encryptedKey, encryptedKeyLen);
    else
    {
        // encrypt wuth ECC symmetric key with recievers public key
        returnCode = ECCencrypt(senderId, recieverrsPublicKeyId, symmetricKey,
                                symmetricKeyLen, encryptedKey, encryptedKeyLen);
}

    return returnCode;
}

/**
 * @brief Performs AES encryption on the first data block.
 * 
 * This function performs AES encryption on the provided input data using the
 * specified chaining mode and symmetric key. The result is written to the output
 * buffer, and a chunk counter is initialized for multi-part encryption.
 * 
 * @param senderId The ID of the sender.
 * @param in The input data to be encrypted.
 * @param inLen The length of the input data.
 * @param[out] out The buffer where the encrypted output will be stored.
 * @param[out] outLen The length of the encrypted output.
 * @param chainingMode The AES chaining mode (e.g., CBC, CTR).
 * @param symmetricKey The symmetric AES key used for encryption.
 * @param symmetricKeyLen The length of the symmetric AES key.
 * @param counter The counter for the chunked encryption process.
 * @return CKR_OK on success or an appropriate error code on failure.
 */
CK_RV performAESEncryption(int senderId, void *in, size_t inLen, void *out,
                           size_t outLen, AESChainingMode chainingMode,
                           std::shared_ptr<unsigned char[]> symmetricKey,
                           AESKeyLength keyLength, size_t counter)
{
    StreamAES *streamAES = FactoryManager::getInstance().create(chainingMode);
    mapToInMiddleEncryptions[senderId] = std::make_pair(streamAES, counter);
    mapToInMiddleEncryptions[senderId].first->encryptStart(
        reinterpret_cast<unsigned char *>(in), inLen,
        reinterpret_cast<unsigned char *&>(out), reinterpret_cast<unsigned int&>(outLen), symmetricKey.get(),
        keyLength);

    return CKR_OK;
}

/**
 * @brief Handles the AES encryption of subsequent chunks of data.
 * 
 * This function continues AES encryption on a subsequent chunk of data using the
 * pre-existing encryption context. It also updates the chunk counter and removes
 * the sender from the encryption map once all chunks are processed.
 * 
 * @param senderId The ID of the sender.
 * @param in The input data chunk to be encrypted.
 * @param inLen The length of the input data chunk.
 * @param[out] out The buffer where the encrypted output will be stored.
 * @param[out] outLen The length of the encrypted output.
 */
void  AEShandleDataChunksEncryption(int senderId, void *in, size_t inLen,
                                   void *out, size_t outLen,
                                   AESChainingMode mode)
{
    mapToInMiddleEncryptions[senderId].first->encryptContinue(
        reinterpret_cast<unsigned char *>(in), inLen,
        reinterpret_cast<unsigned char *&>(out), reinterpret_cast<unsigned int &>(outLen));
}

/**
 * @brief Encrypts data using AES encryption with optional key generation or retrieval.
 * 
 * This function performs AES encryption, either by generating a temporary AES key or
 * retrieving one by key ID. It also handles the continuation of chunked data encryption.
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
 * @param keyId The ID of the key (used when retrieving an existing key).
 * @param generateKeyFlag Indicates whether to generate a new AES key (true) or retrieve an existing one (false).
 * @return CKR_OK on success or an appropriate error code on failure.
 */
CK_RV AESencrypt(int senderId, int receiverId, void *in, size_t inLen,
                 void *out, size_t outLen, AESKeyLength keyLength,
                 AESChainingMode chainingMode, size_t counter,
                 std::string keyId, bool generateKeyFlag,
                 AsymmetricFunction func)
{
    CK_RV returnCode;
    size_t encryptedKeyLength = 0;
    if (mapToInMiddleEncryptions.count(senderId) == 0) {
        size_t symmetricKeyLen = keyLength;
        std::shared_ptr<unsigned char[]> key;
        // Handle key generation or retrieval
        if (generateKeyFlag) {
            key =std::shared_ptr<unsigned char[]>(new unsigned char[keyLength]);
            generateKey(key.get(), keyLength);
            encryptedKeyLength = getEncryptedLengthByAssymFunc(func);
            returnCode = encryptAESkey(
                senderId, receiverId, key.get(), symmetricKeyLen, out,
                getEncryptedLengthByAssymFunc(func),func);
            if (returnCode != CKR_OK) {
                return returnCode;
            }
        }
        else {
            key = TempHsm::getInstance()
                      .getAESKey(senderId, keyId, ENCRYPT)
                      .first;
        }
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
        AEShandleDataChunksEncryption(senderId, in, inLen, out, outLen,
                                      chainingMode);
    }
    //reduce a chunck from the chuncks counter
    mapToInMiddleEncryptions[senderId].second--;

    // if the last chunck of data delete from map
    if (mapToInMiddleEncryptions[senderId].second == 0) {
        auto it = mapToInMiddleEncryptions.find(senderId);
        mapToInMiddleEncryptions.erase(senderId);
    }

    return CKR_OK;
}

/**
 * @brief User-facing function to handle AES encryption using an existing AES key.
 * 
 * This function is the primary function that the user calls to perform AES encryption. 
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

#pragma endregion RSA and ECC

#pragma region SIGN VERIFY
//////////SIGN VERIFY//////////////////////////////////////////////////////////
#pragma region SIGN VERIFY

/**
 * Returns the size of the hashed message based on the specified hash algorithm.
 *
 * @param hashFunc The hash algorithm for which to get the message size.
 * @return Size of the hashed message in bytes.
 * @throws std::invalid_argument if the provided hash function is not supported.
 */
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

/**
 * @brief Retrieves the length of the digital signature.
 * 
 * This function calls the function to get the RSA encrypted length,
 * which corresponds to the size of the digital signature.
 * 
 * @return The length of the digital signature.
 */
size_t getSignatureLength()
{
    return getRSAencryptedLength();
}

/**
 * @brief Checks if the hashing process is complete for the given user ID.
 * 
 * This function verifies if the user ID exists in the mapping and
 * whether the counter for in-progress hashing has reached zero.
 * 
 * @param userId The ID of the user.
 * @return True if the hashing process is complete, false otherwise.
 */
bool isDoneHashing(int userId)
{
    return mapToInMiddleHashing.count(userId) != 0 &&
           mapToInMiddleHashing[userId].second == 0;
}

/**
 * @brief Checks if this is the first time hashing for the given user ID.
 * 
 * This function checks if the user ID is not present in the mapping,
 * indicating that it's the first hashing attempt.
 * 
 * @param userId The ID of the user.
 * @return True if it's the first time hashing, false otherwise.
 */
bool isFirstTimeHash(int userId)
{
    return mapToInMiddleHashing.count(userId) == 0;
}

/**
 * @brief Updates the hash data for the specified user ID.
 * 
 * This function processes a chunk of data, creating a hash instance if it's the first update
 * and appending the data to the ongoing hash calculation. It decrements the counter for remaining data.
 * 
 * @param userId The ID of the user.
 * @param data Pointer to the data to hash.
 * @param dataLen Length of the data.
 * @param hashfunc The hashing algorithm to use.
 * @param counter A counter indicating remaining data chunks.
 * @return CK_RV Return code indicating success or failure of the operation.
 */
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

/**
 * @brief Finalizes the hash computation and retrieves the result for the specified user ID.
 * 
 * This function finalizes the ongoing hash calculation for the user ID, copies the result
 * to the output buffer, and removes the user ID from the mapping.
 * 
 * @param userId The ID of the user.
 * @param out Pointer to the output buffer for the final hash.
 * @param outLen Length of the output buffer.
 * @return CK_RV Return code indicating success or failure of the operation.
 */
CK_RV hashDataFinalize(int userId, void *out, size_t outLen)
{
    std::vector<uint8_t> result;
    CK_RV returnCode = mapToInMiddleHashing[userId].first->finalize(result);
    auto it = mapToInMiddleHashing.find(userId);
    mapToInMiddleHashing.erase(userId);
    memcpy(out, result.data(), outLen);
    return returnCode;
}

/**
 * @brief Updates the signature process with the given data for the specified sender ID.
 * 
 * This function logs the signing action and calls the hash data update function.
 * 
 * @param senderId The ID of the sender.
 * @param data Pointer to the data to sign.
 * @param dataLen Length of the data.
 * @param hashfunc The hashing algorithm to use.
 * @param counter A counter indicating remaining data chunks.
 * @return CK_RV Return code indicating success or failure of the operation.
 */
CK_RV signUpdate(int senderId, void *data, size_t dataLen,
                 SHAAlgorithm hashfunc, int counter)
{
    log(logger::LogLevel::INFO,
        "Signing for user id: " + std::to_string(senderId) +
            " chunk of data.");
    return hashDataUpdate(senderId, data, dataLen, hashfunc, counter);
}

/**
 * @brief Finalizes the signing process for the specified sender ID.
 * 
 * This function finalizes the hashing, encrypts the hash using RSA,
 * and produces the digital signature.
 * 
 * @param senderId The ID of the sender.
 * @param signature Pointer to the output buffer for the signature.
 * @param signatureLen Length of the signature buffer.
 * @param hashfunc The hashing algorithm to use.
 * @param keyId The ID of the key to use for signing.
 * @return CK_RV Return code indicating success or failure of the operation.
 */
CK_RV signFinalize(int senderId, void *signature, size_t signatureLen,
                   SHAAlgorithm hashfunc, std::string keyId)
{
    log(logger::LogLevel::INFO,
        "Signing for user id: " + std::to_string(senderId) +
            " for last chunk of data.");
    size_t hashLen = getHashedMessageSize(hashfunc);
    std::vector<uint8_t> hash(hashLen);
    CK_RV returnCode = hashDataFinalize(senderId, hash.data(), hashLen);
    if (returnCode != CKR_OK)
        return returnCode;
    returnCode = RSAencrypt(senderId, keyId, hash.data(), hashLen, signature,
                            signatureLen);
    return returnCode;
}

/**
 * @brief Updates the verification process with the given data for the specified receiver ID.
 * 
 * This function logs the verification action and calls the hash data update function.
 * 
 * @param recieverId The ID of the receiver.
 * @param data Pointer to the data to verify.
 * @param dataLen Length of the data.
 * @param hashFunc The hashing algorithm to use.
 * @param counter A counter indicating remaining data chunks.
 * @return CK_RV Return code indicating success or failure of the operation.
 */
CK_RV verifyUpdate(int recieverId, void *data, size_t dataLen,
                   SHAAlgorithm hashFunc, size_t counter)
{
    log(logger::LogLevel::INFO,
        "Verifying for user id: " + std::to_string(recieverId) +
            " chunk of data.");
    return hashDataUpdate(recieverId, data, dataLen, hashFunc, counter);
}

/**
 * @brief Finalizes the verification process for the specified receiver ID.
 * 
 * This function finalizes the hashing, decrypts the signature using RSA,
 * and checks if the signature matches the computed hash.
 * 
 * @param recieverId The ID of the receiver.
 * @param signature Pointer to the digital signature to verify.
 * @param signatureLen Length of the signature buffer.
 * @param hashFunc The hashing algorithm to use.
 * @param keyId The ID of the key to use for verification.
 * @return CK_RV Return code indicating success or failure of the operation.
 */
CK_RV verifyFinalize(int recieverId, void *signature, size_t signatureLen,
                     SHAAlgorithm hashFunc, std::string keyId)
{
    log(logger::LogLevel::INFO,
        "Verifying for user id: " + std::to_string(recieverId) +
            " for last chunk of data.");
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
 * @brief Calculates the length of the encrypted data based on the input length.
 * 
 * This function retrieves the encryption configuration for the given sender ID
 * and calculates the total encrypted length, which includes the length of the
 * digital signature, padded encrypted data, and potentially the encrypted symmetric key.
 * 
 * @param senderId The ID of the sender.
 * @param inLen The length of the input data.
 * @param isFirst Indicates if this is the first chunk of data.
 * @return The total length of the encrypted data.
 */
size_t getEncryptedLen(int senderId, size_t inLen, bool isFirst)
{
    // Retrieve the encryption function type for the given sender ID
    CryptoConfig config = TempHsm::getInstance().getUserConfig(senderId);
    // digital signature + encrypted padded data (+ if first chunk: encrypted symmetric key)
    return getAESencryptedLength(inLen, isFirst, config.aesChainingMode) +
           (isFirst ? getEncryptedLengthByAssymFunc(config.asymmetricFunction) : 0);
}

/**
 * @brief Calculates the length of the decrypted data based on the input length.
 * 
 * This function retrieves the encryption configuration for the given sender ID
 * and calculates the length of the decrypted data by accounting for the encrypted
 * length and potential asymmetric function overhead.
 * 
 * @param senderId The ID of the sender.
 * @param inLen The length of the input encrypted data.
 * @param isFirst Indicates if this is the first chunk of data.
 * @return The total length of the decrypted data.
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
 * @brief Encrypts the input data, signs it with a digital signature, according to the user's configuration, and outputs the result.
 *
 * This function first signs the hashed input data using RSA, then encrypts the data using AES.
 * The encryption and signing are performed based on the settings defined in the sender's user configuration.
 * The resulting signature and encrypted data are concatenated and stored in the output buffer.
 *
 * @param senderId The ID of the sender, used for retrieving the appropriate encryption keys and user configuration.
 * @param receiverId The ID of the receiver, used for retrieving the public key for encryption.
 * @param in The input data to be signed and encrypted.
 * @param inLen The length of the input data.
 * @param out The buffer to store the concatenated signature and encrypted data.
 * @param[out] outLen The length of the output data (signature + encrypted data).
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
    returnCode =
        AESencrypt(senderId, receiverId, in, inLen, out, outLen,
                   config.aesKeyLength, config.aesChainingMode, counter,
                   recieversPublicKeyId, true, config.asymmetricFunction);
    if (returnCode != CKR_OK)
        return returnCode;
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
 * @brief Decrypts the input data, verifies its signature, and outputs the result.
 *
 * This function extracts the digital signature and encrypted data from the input buffer,
 * decrypts the data using AES, and verifies the digital signature using RSA. The decryption
 * and signature verification are based on the settings defined in the sender's user configuration.
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
    returnCode = AESdecrypt(senderId, receiverId, in, inLen, out, outLen,
                            config.asymmetricFunction, config.aesKeyLength,
                            config.aesChainingMode, counter, true);
    if (returnCode != CKR_OK)
        return returnCode;
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
