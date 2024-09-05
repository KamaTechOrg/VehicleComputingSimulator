#include "../include/crypto_api.h"
#include <iostream>
#include <iomanip>
#include <map>
std::map<int,std::pair<StreamAES*,size_t>> mapToInMiddleEncryptions;
std::map<int,std::pair<StreamAES*,size_t>> mapToInMiddleDecryptions;

#define DEBUG

constexpr size_t BITS_IN_BYTE = 8;
constexpr size_t ECC_CIPHER_LENGTH = 512;
constexpr size_t IV_LENGTH = 128;
const unsigned int BLOCK_BYTES_LENGTH = 16 * sizeof(unsigned char);
const unsigned int RSA_KEY_LENGTH = 1024;


void printBufferHex(const uint8_t *buffer, size_t len, std::string message)
{
    #ifdef DEBUG
    std::cout << message << std::endl;
    for (size_t i = 0; i < len; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(buffer[i]) << " ";
        // Print a new line every 16 bytes for better readability
        if ((i + 1) % 16 == 0) {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
    // Reset the stream format back to decimal
    std::cout << std::dec;
    #endif
}

////////////////// the encrypt function that the user calls//////////////////
//the out will be : (based on the func)
//1st option:                           // 2nd option:
// encrypted aes key -> rsa/ecc
// IV+encrypted data                     // data
// signature= encrypted hash-> rsa/ecc // signature= encrypted hash-> rsa/ecc
int encrypt(int senderId, int receiverId, void *in, size_t inLen, void *out,
            size_t &outLen, size_t counter)
{
    in = static_cast<uint8_t *>(in);
    encryptionFunction func = Hsm::getEncryptionFunctionType(senderId);

    size_t signatureLen, encryptedKeyAndDataLen;
    uint8_t *signature, *encryptedKeyAndData;
    //1.  sign the hashed data

    signatureLen = getAsymmSignLen(func);
    signature = new uint8_t[signatureLen];
    sign(senderId, in, inLen, signature, signatureLen, func);
    //printBufferHex(signature, signatureLen, "digital signature=encrypted hash");
    if (isSignatureOnly(func)) {
        outLen = inLen + signatureLen;
        // copy data
        std::memcpy(static_cast<uint8_t *>(out), in, inLen);
        //copy signature
        std::memcpy(static_cast<uint8_t *>(out) + inLen, signature,
                    signatureLen);
        delete[] signature;
        return 0;
    }
    encryptedKeyAndDataLen = getSymmEncLen(inLen) + getAsymmEncLen(func);
    encryptedKeyAndData = new uint8_t[encryptedKeyAndDataLen];
    performEncryption(receiverId, reinterpret_cast<uint8_t *>(in), inLen,
                      encryptedKeyAndData, encryptedKeyAndDataLen, func, senderId, counter);
    outLen = encryptedKeyAndDataLen + signatureLen;
    // Copy encrypted symmetric key and encrypted data
    std::memcpy(static_cast<uint8_t *>(out), encryptedKeyAndData,
                encryptedKeyAndDataLen);
    // Copy signature
    std::memcpy(static_cast<uint8_t *>(out) + encryptedKeyAndDataLen, signature,
                signatureLen);

    // Free allocated memory
    delete[] signature;
    delete[] encryptedKeyAndData;
    return 0;
}

////////////////// the decrypt function that the user calls////////////////////
//the in includes : (based on the func)
//1st option:                           // 2nd option:
// encrypted aes key -> rsa/ecc
// IV+encrypted data                     // data
// signature= encrypted hash-> rsa/ecc // signature= encrypted hash-> rsa/ecc
// the out will be the plain data aftergoing through all the decryption stages 
int decrypt(int senderId, int receiverId, void *in, size_t inLen, void *out,
             size_t &outLen, size_t counter)
{
    uint8_t * uin = reinterpret_cast<uint8_t *>(in);
    encryptionFunction func = Hsm::getEncryptionFunctionType(senderId);
    size_t dataLen, decryptedKeyLen, signatureLen, encrypted;
    uint8_t *data, *signature;

    signatureLen = getAsymmSignLen(func);
    signature = new uint8_t[signatureLen];
    memcpy(signature, uin + inLen - signatureLen, signatureLen);
    if (isSignatureOnly(func)) {
        try {
            verify(senderId, in, inLen - signatureLen, signature, signatureLen,
                   func);
        }
        catch (const std::exception &e) {
            delete[] signature;
            throw e;
        }
        delete[] signature;
        return 0;
    }

    size_t encryptedKeyAndDataLen = inLen - signatureLen;
    size_t decryptedDataLen = encryptedKeyAndDataLen - getAsymmEncLen(func);
    uint8_t *decryptedData = new uint8_t[decryptedDataLen];

    uint8_t *encryptedKeyAndData =
        extractFromPointer(in, encryptedKeyAndDataLen);

    performDecryption(receiverId, encryptedKeyAndData, encryptedKeyAndDataLen,
                      decryptedData, decryptedDataLen, func, senderId, counter);
    try {
        verify(senderId, decryptedData, decryptedDataLen, signature,
               signatureLen, func);
    }
    catch (const std::exception &e) {
        delete[] signature;
        delete[] encryptedKeyAndData;
        delete[] decryptedData;
        throw e;
    }

    // Free dynamically allocated memory
    delete[] signature;
    delete[] encryptedKeyAndData;
    delete[] decryptedData;
    return 0;
}

//function to encrypt data with aes and encrypt symmetric key with recievers public key
// the out will be encrypted Key + encrypted data.
void performEncryption(int recieverId, uint8_t *in, size_t inLen, uint8_t *out,
                       size_t &outLen, encryptionFunction func, int senderId, size_t counter)
{
    unsigned int encryptedDataLen;
    size_t symmetricKeyLen, encryptedKeyLen;
    uint8_t *symmetricKey, *encryptedData, *encryptedKey;

    symmetricKeyLen = convertAESKeyLengthToInt(mapToAESKeyLength(func)) /
                      BITS_IN_BYTE;  // Example: 256-bit key = 32 bytes

    symmetricKey = new uint8_t[symmetricKeyLen];
    symmetricKey = generateKey(mapToAESKeyLength(func));
    //printBufferHex(symmetricKey, symmetricKeyLen, "symmetric key");
    encryptedDataLen = getSymmEncLen(inLen);
    encryptedData = new uint8_t[encryptedDataLen];
    if (mapToInMiddleEncryptions.count(senderId) == 0) {
       StreamAES* streamAES  = FactoryManager::getInstance().create(AESChainingMode::CBC);
       streamAES->encryptStart(in, inLen, encryptedData, encryptedDataLen, symmetricKey, mapToAESKeyLength(func));
       mapToInMiddleEncryptions[senderId] = std::make_pair(streamAES, counter);
    }
    else
        mapToInMiddleEncryptions[senderId].first->encryptContinue(in,inLen, encryptedData, encryptedDataLen);
    mapToInMiddleEncryptions[senderId].second--;
    if(mapToInMiddleEncryptions[senderId].second == 0){
        auto it = mapToInMiddleEncryptions.find(senderId);
        mapToInMiddleEncryptions.erase(senderId);
    }
    //printBufferHex(encryptedData, encryptedDataLen, "encrypted data+iv");
    //encrypt symmetric key with recievers public key
    encryptedKeyLen = getAsymmEncLen(func);
    encryptedKey = new uint8_t[encryptedKeyLen];
    if (Hsm::isRsaEncryption(func)) {
        // Encrypt  key with the reciever public key
        rsaEncrypt(symmetricKey, symmetricKeyLen,
                   ((RsaKeys *)Hsm::getKey(recieverId))->n,
                   ((RsaKeys *)Hsm::getKey(recieverId))->pubKey, encryptedKey,
                   encryptedKeyLen, RSA_KEY_LENGTH);
    }
    else {
        encryptedKeyLen = 512;
        unsigned char *buffer = new unsigned char[encryptedKeyLen];
        auto enKey =
            encryptECC(std::string(in, in + inLen),
                       ((EccKeys *)Hsm::getKey(recieverId))->publicKey);
        memcpy(encryptedKey, static_cast<void *>(&enKey), encryptedKeyLen);
    }
    //printBufferHex(encryptedKey, encryptedKeyLen, "encrypted key");
    outLen = encryptedDataLen + encryptedKeyLen;
    // Copy encrypted symmetric key
    std::memcpy(out, encryptedKey, encryptedKeyLen);
    // Copy encrypted data
    std::memcpy(out + encryptedKeyLen, encryptedData, encryptedDataLen);

    delete[] encryptedData;
    delete[] encryptedKey;
    delete[] symmetricKey;
}

//function to decrypt symmetric key with reciever private key and decrypt data with aes
//the data will be the plain decrypted data
void performDecryption(int recieverId, uint8_t *encryptedKeyAndData,
                       size_t encryptedKeyAndDataLen, uint8_t *data,
                       size_t &dataLen, encryptionFunction func, int senderId, size_t counter)
{
    size_t encryptedKeyLen = getAsymmEncLen(func);
    size_t encryptedDataLen = encryptedKeyAndDataLen - encryptedKeyLen;

    unsigned int decryptedDataLen = encryptedDataLen;
    uint8_t *decryptedData = new uint8_t[decryptedDataLen];
    // Extract components
    uint8_t *encryptedKey = 
        extractFromPointer(encryptedKeyAndData, encryptedKeyLen);
    //printBufferHex(encryptedKey, encryptedKeyLen, "encryptedKey");
    uint8_t *encryptedData = extractFromPointer(
        encryptedKeyAndData + encryptedKeyLen, encryptedDataLen);
    //printBufferHex(encryptedData, encryptedDataLen, "encryptedData");
    size_t decryptedKeyLen = convertAESKeyLengthToInt(mapToAESKeyLength(func));
    uint8_t *decryptedKey = new uint8_t[decryptedKeyLen];
    if (Hsm::isRsaEncryption(func)) {
        rsaDecrypt(encryptedKey, encryptedKeyLen,
                   ((RsaKeys *)Hsm::getKey(recieverId))->n,
                   ((RsaKeys *)Hsm::getKey(recieverId))->priKey, decryptedKey,
                   &decryptedKeyLen, RSA_KEY_LENGTH);
        //printBufferHex(decryptedKey, decryptedKeyLen, "decrypted key");
    }
    else {
        //decrypt the key with ecc
        decryptedKey = reinterpret_cast<uint8_t *>(
            decryptECC(*(reinterpret_cast<EncryptedMessage *>(encryptedKey)),
                       ((EccKeys *)Hsm::getKey(recieverId))->privateKey)
                .data());
    }
    
    try{
        if (mapToInMiddleDecryptions.count(senderId) == 0) {
       StreamAES* streamAES  = FactoryManager::getInstance().create(AESChainingMode::CBC);
       streamAES->decryptStart(encryptedData, encryptedDataLen, decryptedData, decryptedDataLen, decryptedKey, mapToAESKeyLength(func));
       mapToInMiddleEncryptions[senderId] = std::make_pair(streamAES, counter);
    }
    else
        mapToInMiddleDecryptions[senderId].first->decryptContinue(encryptedData,encryptedDataLen, decryptedData, decryptedDataLen);
    if(mapToInMiddleDecryptions[senderId].second == 0){
        auto it = mapToInMiddleDecryptions.find(senderId);
        mapToInMiddleDecryptions.erase(senderId);
    }
    std::memcpy(data, decryptedData, decryptedDataLen);
    dataLen = decryptedDataLen;
    }
    catch(std::length_error){
        
    }
    //printBufferHex(decryptedData, decryptedDataLen, "decryptedData");
    delete[] encryptedData;
    delete[] encryptedKey;
    delete[] decryptedKey;
    delete[] decryptedData;
}

//function to sign data, creates signature rsa/ecc
void sign(int senderId, void *data, size_t dataLen, void *signature,
          size_t &signatureLen, encryptionFunction func)
{
    //1. hash the in
    uint8_t *hash = new uint8_t[(getHashedSize(func)/BITS_IN_BYTE)];
    std::vector<uint8_t> hashVec = sha256_compute(std::vector<uint8_t>(
        static_cast<uint8_t *>(data), static_cast<uint8_t *>(data) + dataLen));
    memcpy(hash, hashVec.data(), (getHashedSize(func)/BITS_IN_BYTE));
    //printBufferHex(hash, hashLength, "hashed message");

    //2. encrypt the hash with sender's private key
    rsaEncrypt(hash, getHashedSize(func)/BITS_IN_BYTE, ((RsaKeys *)Hsm::getKey(senderId))->n,
                ((RsaKeys *)Hsm::getKey(senderId))->priKey,
                static_cast<uint8_t *>(signature), signatureLen,
                RSA_KEY_LENGTH); 
}

//function to verify data, throws error if the hashed message != decrypted signature
void verify(int senderId, void *data, size_t dataLen, void *signature,
            size_t signatureLen, encryptionFunction func)
{
    //1. hash the data
    uint8_t *hash = new uint8_t[(getHashedSize(func)/BITS_IN_BYTE)];
    std::vector<uint8_t> hashVec = sha256_compute(std::vector<uint8_t>(
        static_cast<uint8_t *>(data), static_cast<uint8_t *>(data) + dataLen));
    memcpy(hash, hashVec.data(), (getHashedSize(func)/BITS_IN_BYTE));
    bool res = false;
    //2. decrypt digital signature with the sender public key and compare the 2 hashes
    if (Hsm::isRsaEncryption(func)) {
        size_t decryptedHahLen = rsaGetDecryptedLen(RSA_KEY_LENGTH);
        uint8_t *decryptedHash = new uint8_t[decryptedHahLen];
        rsaDecrypt(reinterpret_cast<const uint8_t *>(signature), signatureLen,
                   ((RsaKeys *)Hsm::getKey(senderId))->n,
                   ((RsaKeys *)Hsm::getKey(senderId))->pubKey, decryptedHash,
                   &decryptedHahLen, RSA_KEY_LENGTH);
        //printBufferHex(hash, hashLength, "hash on the original message");
        //printBufferHex(decryptedHash, hashLength, "decrypted signature");

        if (memcmp(hash, decryptedHash, getHashedSize(func)/BITS_IN_BYTE) == 0) {
            res = true;
        }
        delete[] decryptedHash;
    }
    delete[] hash;
    if (!res)
        throw std::runtime_error(
            "oh no!!!! message not accurate, some thing went wrong :( "
            "verifying failed...");
}

//function to extract the beginning of 'in' pointer
uint8_t *extractFromPointer(const void *in, int length)
{
    uint8_t *beginning = new uint8_t[length];
    std::memcpy(beginning, in, length);
    return beginning;
}

size_t getSymmEncLen(size_t inLen)
{
    return (IV_LENGTH/BITS_IN_BYTE) +
           (((inLen + BLOCK_BYTES_LEN - 1) / BLOCK_BYTES_LEN) * BLOCK_BYTES_LEN);
}

//get the size required to allocate for encryptaed message with encrypted symmetric key and digital signature
//in is original data
size_t getEncryptedLen(int senderId, size_t inLen)
{
    encryptionFunction func = Hsm::getEncryptionFunctionType(senderId);
    if (isSignatureOnly(func))
        //data + signature
        return inLen + getAsymmSignLen(func);
    else
        // encrypted symmetric key +encrypted padded data+ digital signature
        return getAsymmEncLen(func) + getSymmEncLen(inLen) +
               getAsymmSignLen(func);
}

// Function to get the size required to allocate for decrypted message (includes the AES padding...)
// 'inLen' is the length of the encrypted data
size_t getDecryptedLen(int senderId, size_t inLen)
{
    // Retrieve the encryption function type for the given sender ID
    encryptionFunction func = Hsm::getEncryptionFunctionType(senderId);

    // Check if the function corresponds to a signature-only mode
    if (isSignatureOnly(func))
        // Data + signature: Remove the signature length from the input length
        return inLen - getAsymmSignLen(func);
    else
            return inLen - getAsymmEncLen(func) - getAsymmSignLen(func);
}

// Function to check if the encryption function is for signature only or includes encryption with signing
bool isSignatureOnly(encryptionFunction func)
{
    switch (func) {
        case SIGNATURE_SHA3_512_CFB:
        case SIGNATURE_SHA3_512_CTR:
        case SIGNATURE_SHA3_512_ECB:
        case SIGNATURE_SHA3_512_OFB:
        case SIGNATURE_SHA3_512_CBC:
        case SIGNATURE_SHA256_CFB:
        case SIGNATURE_SHA256_CTR:
        case SIGNATURE_SHA256_ECB:
        case SIGNATURE_SHA256_OFB:
        case SIGNATURE_SHA256_CBC:
            return true;  // These are signature-only functions
        default:
            return false; // These include both encryption and signing
    }
}

// Function to map EncryptionFunc to AESKeyLength
AESKeyLength mapToAESKeyLength(encryptionFunction func)
{
    switch (func) {
        case SIGNATURE_SHA3_512_CBC:
        case SIGNATURE_SHA3_512_CFB:
        case SIGNATURE_SHA3_512_CTR:
        case SIGNATURE_SHA3_512_ECB:
        case SIGNATURE_SHA3_512_OFB:
        case ENCRYPT_128_AND_SIGN_SHA3_512_CBC:
        case ENCRYPT_128_AND_SIGN_SHA3_512_CFB:
        case ENCRYPT_128_AND_SIGN_SHA3_512_CTR:
        case ENCRYPT_128_AND_SIGN_SHA3_512_ECB:
        case ENCRYPT_128_AND_SIGN_SHA3_512_OFB:
            return AESKeyLength::AES_128;
        
        case ENCRYPT_192_AND_SIGN_SHA3_512_CBC:
        case ENCRYPT_192_AND_SIGN_SHA3_512_CFB:
        case ENCRYPT_192_AND_SIGN_SHA3_512_CTR:
        case ENCRYPT_192_AND_SIGN_SHA3_512_ECB:
        case ENCRYPT_192_AND_SIGN_SHA3_512_OFB:
            return AESKeyLength::AES_192;

        case ENCRYPT_256_AND_SIGN_SHA3_512_CBC:
        case ENCRYPT_256_AND_SIGN_SHA3_512_CFB:
        case ENCRYPT_256_AND_SIGN_SHA3_512_CTR:
        case ENCRYPT_256_AND_SIGN_SHA3_512_ECB:
        case ENCRYPT_256_AND_SIGN_SHA3_512_OFB:
            return AESKeyLength::AES_256;

        case SIGNATURE_SHA256_CBC:
        case SIGNATURE_SHA256_CFB:
        case SIGNATURE_SHA256_CTR:
        case SIGNATURE_SHA256_ECB:
        case SIGNATURE_SHA256_OFB:
        case ENCRYPT_128_AND_SIGN_SHA256_CBC:
        case ENCRYPT_128_AND_SIGN_SHA256_CFB:
        case ENCRYPT_128_AND_SIGN_SHA256_CTR:
        case ENCRYPT_128_AND_SIGN_SHA256_ECB:
        case ENCRYPT_128_AND_SIGN_SHA256_OFB:
            return AESKeyLength::AES_128;
        
        case ENCRYPT_192_AND_SIGN_SHA256_CBC:
        case ENCRYPT_192_AND_SIGN_SHA256_CFB:
        case ENCRYPT_192_AND_SIGN_SHA256_CTR:
        case ENCRYPT_192_AND_SIGN_SHA256_ECB:
        case ENCRYPT_192_AND_SIGN_SHA256_OFB:
            return AESKeyLength::AES_192;

        case ENCRYPT_256_AND_SIGN_SHA256_CBC:
        case ENCRYPT_256_AND_SIGN_SHA256_CFB:
        case ENCRYPT_256_AND_SIGN_SHA256_CTR:
        case ENCRYPT_256_AND_SIGN_SHA256_ECB:
        case ENCRYPT_256_AND_SIGN_SHA256_OFB:
            return AESKeyLength::AES_256;

        default:
            throw std::invalid_argument("Invalid EncryptionFunction");
    }
}

//Function to map AESKeyLength to length in int
size_t convertAESKeyLengthToInt(AESKeyLength keyLength)
{
    switch (keyLength) {
        case AESKeyLength::AES_128:
            return 128;
        case AESKeyLength::AES_192:
            return 192;
        case AESKeyLength::AES_256:
            return 256;
        default:
            throw std::invalid_argument("Invalid AESKeyLength");
    }
}

//Function to map hash function to length in int
size_t getHashedSize(hashFunction hashFunc)
{
    switch (hashFunc) {
        case hashFunction::SHA_256:
            return 256;
        case hashFunction::SHA3_512:
            return 512;
        default:
            throw std::invalid_argument("Invalid hash function");
    }
}
// Function to map encryptionFunction to hash size in bits
size_t getHashedSize(encryptionFunction func)
{
    switch (func) {
        // SHA256-related functions
        case SIGNATURE_SHA256_CFB:
        case SIGNATURE_SHA256_CTR:
        case SIGNATURE_SHA256_ECB:
        case SIGNATURE_SHA256_OFB:
        case SIGNATURE_SHA256_CBC:
        case ENCRYPT_128_AND_SIGN_SHA256_CBC:
        case ENCRYPT_128_AND_SIGN_SHA256_CFB:
        case ENCRYPT_128_AND_SIGN_SHA256_CTR:
        case ENCRYPT_128_AND_SIGN_SHA256_ECB:
        case ENCRYPT_128_AND_SIGN_SHA256_OFB:
        case ENCRYPT_192_AND_SIGN_SHA256_CBC:
        case ENCRYPT_192_AND_SIGN_SHA256_CFB:
        case ENCRYPT_192_AND_SIGN_SHA256_CTR:
        case ENCRYPT_192_AND_SIGN_SHA256_ECB:
        case ENCRYPT_192_AND_SIGN_SHA256_OFB:
        case ENCRYPT_256_AND_SIGN_SHA256_CBC:
        case ENCRYPT_256_AND_SIGN_SHA256_CFB:
        case ENCRYPT_256_AND_SIGN_SHA256_CTR:
        case ENCRYPT_256_AND_SIGN_SHA256_ECB:
        case ENCRYPT_256_AND_SIGN_SHA256_OFB:
            return 256; // SHA256 has a hash size of 256 bits

        // SHA3-512-related functions
        case SIGNATURE_SHA3_512_CFB:
        case SIGNATURE_SHA3_512_CTR:
        case SIGNATURE_SHA3_512_ECB:
        case SIGNATURE_SHA3_512_OFB:
        case SIGNATURE_SHA3_512_CBC:
        case ENCRYPT_128_AND_SIGN_SHA3_512_CBC:
        case ENCRYPT_128_AND_SIGN_SHA3_512_CFB:
        case ENCRYPT_128_AND_SIGN_SHA3_512_CTR:
        case ENCRYPT_128_AND_SIGN_SHA3_512_ECB:
        case ENCRYPT_128_AND_SIGN_SHA3_512_OFB:
        case ENCRYPT_192_AND_SIGN_SHA3_512_CBC:
        case ENCRYPT_192_AND_SIGN_SHA3_512_CFB:
        case ENCRYPT_192_AND_SIGN_SHA3_512_CTR:
        case ENCRYPT_192_AND_SIGN_SHA3_512_ECB:
        case ENCRYPT_192_AND_SIGN_SHA3_512_OFB:
        case ENCRYPT_256_AND_SIGN_SHA3_512_CBC:
        case ENCRYPT_256_AND_SIGN_SHA3_512_CFB:
        case ENCRYPT_256_AND_SIGN_SHA3_512_CTR:
        case ENCRYPT_256_AND_SIGN_SHA3_512_ECB:
        case ENCRYPT_256_AND_SIGN_SHA3_512_OFB:
            return 512; // SHA3-512 has a hash size of 512 bits

        default:
            throw std::invalid_argument("Invalid hash function");
    }
}

size_t getAsymmEncLen(encryptionFunction func)
{
    return getAsymmKeyBitsEnc(func) / BITS_IN_BYTE;
}

size_t getAsymmSignLen(encryptionFunction func)
{
    return getAsymmKeyBitsSign(func) / BITS_IN_BYTE;
}





///////////////////////////////////////////////////////////////////////////

void ECCencrypt(int receiverId, void *in, size_t inLen, void *out,
            size_t &outLen)
{
    std::vector<uint8_t> inVec(static_cast<uint8_t*>(in), static_cast<uint8_t*>(in) + inLen);
    // Encrypt the message
    auto cipher = encryptECC(inVec, getECCPublicKey(receiverId));
    out=static_cast<void *>(&cipher);
    outLen=ECC_CIPHER_LENGTH;
}

void ECCdecrypt(int receiverId, void *in, size_t inLen, void *out,
            size_t &outLen)
{
    // Decrypt the message
    out = reinterpret_cast<uint8_t *>(decryptECC(*(reinterpret_cast<EncryptedMessage *>(in)), getECCPrivateKey(receiverId)).data());
    outLen=ECC_CIPHER_LENGTH;
}

//function to sign data, creates signature rsa
void sign1(int senderId, void *data, size_t dataLen, void *signature,
          size_t &signatureLen, hashFunction hashFunc)
{
    //1. hash the in
    uint8_t *hash = new uint8_t[(getHashedSize(hashFunc)/BITS_IN_BYTE)];
    std::vector<uint8_t> hashVec = sha256_compute(std::vector<uint8_t>(
        static_cast<uint8_t *>(data), static_cast<uint8_t *>(data) + dataLen));
    memcpy(hash, hashVec.data(), (getHashedSize(hashFunc)/BITS_IN_BYTE));
    //printBufferHex(hash, hashLength, "hashed message");

    //2. encrypt the hash with sender's private key
    rsaEncrypt(hash, getHashedSize(hashFunc)/BITS_IN_BYTE, ((RsaKeys *)Hsm::getKey(senderId))->n,
                ((RsaKeys *)Hsm::getKey(senderId))->priKey,
                static_cast<uint8_t *>(signature), signatureLen,
                RSA_KEY_LENGTH);   
}

//function to verify data, throws error if the hashed message != decrypted signature
void verify1(int senderId, void *data, size_t dataLen, void *signature,
            size_t &signatureLen, hashFunction hashFunc)
{
    //1. hash the data
    uint8_t *hash = new uint8_t[(getHashedSize(hashFunc)/BITS_IN_BYTE)];
    std::vector<uint8_t> hashVec = sha256_compute(std::vector<uint8_t>(
        static_cast<uint8_t *>(data), static_cast<uint8_t *>(data) + dataLen));
    memcpy(hash, hashVec.data(), (getHashedSize(hashFunc)/BITS_IN_BYTE));
    bool res = false;
    //2. decrypt digital signature with the sender public key and compare the 2 hashes
        size_t decryptedHahLen = rsaGetDecryptedLen(RSA_KEY_LENGTH);
        uint8_t *decryptedHash = new uint8_t[decryptedHahLen];
        rsaDecrypt(reinterpret_cast<const uint8_t *>(signature), signatureLen,
                   ((RsaKeys *)Hsm::getKey(senderId))->n,
                   ((RsaKeys *)Hsm::getKey(senderId))->pubKey, decryptedHash,
                   &decryptedHahLen, RSA_KEY_LENGTH);
        //printBufferHex(hash, hashLength, "hash on the original message");
        //printBufferHex(decryptedHash, hashLength, "decrypted signature");
        if (memcmp(hash, decryptedHash, (getHashedSize(hashFunc)/8)) == 0) {
            res = true;
        }
        delete[] decryptedHash;
    delete[] hash;
    if (!res)
        throw std::runtime_error(
            "oh no!!!! message not accurate, some thing went wrong :( "
            "verifying failed...");
}

unsigned char* generateKeyAES(AESKeyLength keyLength)
{
    return generateKey(keyLength);
}

//encrypts block of size BLOCK_BYTES_LENGTH
void AESencrypt(AESChainingMode mode, AESKeyLength keyLength, size_t countBlocks, unsigned char* key, int senderID,
                void *in, void *out, size_t &outLen)
{
    // Create a factory instance
    StreamAES* streamAES = FactoryManager::getInstance().create(mode);
    //TODO: check if streamAES is nullptr and return code

    unsigned char* encrypted = nullptr;
    unsigned int outLenEncrypted = 0;

    //if senderID  not in map:
        streamAES->encryptStart(static_cast<unsigned char*>(in), BLOCK_BYTES_LENGTH, encrypted, outLenEncrypted, key, keyLength);
    //else 
        streamAES->encryptContinue(static_cast<unsigned char*>(in), BLOCK_BYTES_LENGTH, encrypted, outLenEncrypted);

    std::memcpy(out, encrypted, outLenEncrypted);
    delete[] encrypted;
   // delete streamAES;
}

void AESdecrypt(AESChainingMode mode, AESKeyLength keyLength, unsigned char* key, int receiverId,
             void *in, size_t inLen, void *out, size_t &outLen) {

    // Create a factory instance
    StreamAES* streamAES = FactoryManager::getInstance().create(mode);
    //TODO: check if streamAES is nullptr and return code

    unsigned char* decrypted = nullptr;
    unsigned int outLenDecrypted = 0;

    //if senderID  not in map:
        streamAES->decryptStart(static_cast<unsigned char*>(in), inLen, decrypted, outLenDecrypted, key, keyLength);
    //else 
        streamAES->decryptContinue(static_cast<unsigned char*>(in), inLen, decrypted, outLenDecrypted);

    std::memcpy(out, decrypted, outLenDecrypted);
    delete[] decrypted;
    // delete streamAES;
}