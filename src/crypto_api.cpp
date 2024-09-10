#include "../include/crypto_api.h"
#include <iostream>
#include <iomanip>
#include <map>
std::map<int,std::pair<StreamAES*,size_t>> mapToInMiddleEncryptions;
std::map<int,std::pair<StreamAES*,size_t>> mapToInMiddleDecryptions;

#define DEBUG

constexpr size_t BITS_IN_BYTE = 8;
constexpr size_t ECC_CIPHER_LENGTH = 512;

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
    if (func == SIGNATURE_ECC || func == SIGNATURE_RSA) {
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
                      encryptedKeyAndData, encryptedKeyAndDataLen, func,senderId);
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
    if (func == SIGNATURE_ECC || func == SIGNATURE_RSA) {
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
                      decryptedData, decryptedDataLen, func, senderId);
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
                   encryptedKeyLen, getAsymmKeyBitsEnc(func));
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
                   &decryptedKeyLen, getAsymmKeyBitsEnc(func));
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
    uint8_t *hash = new uint8_t[hashLength];
    std::vector<uint8_t> hashVec = sha256_compute(std::vector<uint8_t>(
        static_cast<uint8_t *>(data), static_cast<uint8_t *>(data) + dataLen));
    memcpy(hash, hashVec.data(), hashLength);
    //printBufferHex(hash, hashLength, "hashed message");

    //2. encrypt the hash with sender's private key
    if (Hsm::isRsaEncryption(func)) {
        rsaEncrypt(hash, hashLength, ((RsaKeys *)Hsm::getKey(senderId))->n,
                   ((RsaKeys *)Hsm::getKey(senderId))->priKey,
                   static_cast<uint8_t *>(signature), signatureLen,
                   getAsymmKeyBitsSign(func));
    }
    // else {
    //     auto signWithECC =
    //         signMessageECC(std::vector<uint8_t>(hash, hash + hashLength),
    //                        ((EccKeys *)Hsm::getKey(senderId))->privateKey);
    //     signature = static_cast<void *>(&signWithECC);
    //     //TODO: signatureLen=???
    // }
}

//function to verify data, throws error if the hashed message != decrypted signature
void verify(int senderId, void *data, size_t dataLen, void *signature,
            size_t signatureLen, encryptionFunction func)
{
    //1. hash the data
    uint8_t *hash = new uint8_t[hashLength];
    std::vector<uint8_t> hashVec = sha256_compute(std::vector<uint8_t>(
        static_cast<uint8_t *>(data), static_cast<uint8_t *>(data) + dataLen));
    memcpy(hash, hashVec.data(), hashLength);
    bool res = false;
    //2. decrypt digital signature with the sender public key and compare the 2 hashes
    if (Hsm::isRsaEncryption(func)) {
        size_t decryptedHahLen = rsaGetDecryptedLen(getAsymmKeyBitsSign(func));
        uint8_t *decryptedHash = new uint8_t[decryptedHahLen];
        rsaDecrypt(reinterpret_cast<const uint8_t *>(signature), signatureLen,
                   ((RsaKeys *)Hsm::getKey(senderId))->n,
                   ((RsaKeys *)Hsm::getKey(senderId))->pubKey, decryptedHash,
                   &decryptedHahLen, getAsymmKeyBitsSign(func));
        //printBufferHex(hash, hashLength, "hash on the original message");
        //printBufferHex(decryptedHash, hashLength, "decrypted signature");

        if (memcmp(hash, decryptedHash, hashLength) == 0) {
            res = true;
        }
        delete[] decryptedHash;
    }
    // else {
    //     if (verifySignatureECC(
    //             std::vector<uint8_t>(hash, hash + hashLength),
    //             static_cast<std::pair<mpz_class, mpz_class> *>(signature)
    //                 ->first,
    //             static_cast<std::pair<mpz_class, mpz_class> *>(signature)
    //                 ->second,
    //             ((EccKeys *)Hsm::getKey(senderId))->publicKey)) {
    //         res = true;
    //     }
    // }
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
    return ivLength +
           (((inLen + blockBytesLen - 1) / blockBytesLen) * blockBytesLen);
}

//get the size required to allocate for encryptaed message with encrypted symmetric key and digital signature
//in is original data
size_t getEncryptedLen(int senderId, size_t inLen)
{
    encryptionFunction func = Hsm::getEncryptionFunctionType(senderId);
    if (func == SIGNATURE_RSA || func == SIGNATURE_ECC)
        //data + signature
        return inLen + getAsymmSignLen(func);
    else
        // encrypted symmetric key +encrypted padded data+ digital signature
        return getAsymmEncLen(func) + getSymmEncLen(inLen) +
               getAsymmSignLen(func);
}

//get the size required to allocate for decryptaed message (includes the aes padding...)
// in is encrypted data
size_t getDecryptedLen(int senderId, size_t inLen)
{
    encryptionFunction func = Hsm::getEncryptionFunctionType(senderId);
    if (func == SIGNATURE_RSA || func == SIGNATURE_ECC)
        //data + signature
        return inLen - getAsymmSignLen(func);
    else
        //encrypted symmetric key +encrypted data(+IV)+ digital signature
        //remove  and digital signature and ecrypted key- leave padded encrypted data
        return inLen - getAsymmEncLen(func) - getAsymmSignLen(func);
}

//Function to map EncryptionFunc to AESKeyLength
AESKeyLength mapToAESKeyLength(encryptionFunction func)
{
    switch (func) {
        case ENCRYPT_128_AND_SIGN_RSA:
        case ENCRYPT_128_AND_SIGN_ECC:
            return AESKeyLength::AES_128;
        case ENCRYPT_192_AND_SIGN_RSA:
        case ENCRYPT_192_AND_SIGN_ECC:
            return AESKeyLength::AES_192;
        case ENCRYPT_256_AND_SIGN_RSA:
        case ENCRYPT_256_AND_SIGN_ECC:
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

size_t getAsymmKeyBitsEnc(encryptionFunction func)
{
    if (Hsm::isRsaEncryption(func)) {
        return 1024;
    }
    else {
        return 512;
    }
}

size_t getAsymmKeyBitsSign(encryptionFunction func)
{
    if (Hsm::isRsaEncryption(func)) {
        return 1024;
    }
    else {
        return 256;
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
    // Encrypt the message
    out = encryptECC(in, getECCPublicKey(receiverId));
    outLen=ECC_CIPHER_LENGTH;    
}

void ECCdecrypt(int receiverId, void *in, size_t inLen, void *out,
            size_t &outLen)
{
    // Decrypt the message
    out = decryptECC(in, getECCPrivateKey(receiverId));
    outLen=ECC_CIPHER_LENGTH;
}

//function to sign data, creates signature rsa
void sign1(int senderId, void *data, size_t dataLen, void *signature,
          size_t &signatureLen, encryptionFunction func)
{
    //1. hash the in
    uint8_t *hash = new uint8_t[hashLength];
    std::vector<uint8_t> hashVec = sha256_compute(std::vector<uint8_t>(
        static_cast<uint8_t *>(data), static_cast<uint8_t *>(data) + dataLen));
    memcpy(hash, hashVec.data(), hashLength);
    //printBufferHex(hash, hashLength, "hashed message");

    //2. encrypt the hash with sender's private key
    if (Hsm::isRsaEncryption(func)) {
        rsaEncrypt(hash, hashLength, ((RsaKeys *)Hsm::getKey(senderId))->n,
                   ((RsaKeys *)Hsm::getKey(senderId))->priKey,
                   static_cast<uint8_t *>(signature), signatureLen,
                   getAsymmKeyBitsSign(func));
    }
    // else {
    //     auto signWithECC =
    //         signMessageECC(std::vector<uint8_t>(hash, hash + hashLength),
    //                        ((EccKeys *)Hsm::getKey(senderId))->privateKey);
    //     signature = static_cast<void *>(&signWithECC);
    //     //TODO: signatureLen=???
    // }
}