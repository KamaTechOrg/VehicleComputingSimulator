#ifndef __CRYPTO_API_H__
#define __CRYPTO_API_H__

#include <cstddef>
#include <string>
#include <stdexcept>
#include <thread>
#include <future>

#include "aes_stream_factory.h"
#include "sha256.h"
//#include "rsa.h"
#include "hsm.h"
#include "ecc.h"
#include "key_manager.h"

int encrypt(int senderId, int receiverId, void *in, size_t inLen, void *out,
            size_t &outLen, size_t counter);

int decrypt(int senderId, int receiverId, void *in, size_t inLen, void *out,
             size_t &outLen, size_t counter);

size_t getEncryptedLen(int senderId, size_t length);

size_t getDecryptedLen(int senderId, size_t encryptedLength);

AESKeyLength mapToAESKeyLength(encryptionFunction func);

void performEncryption(int recieverId, uint8_t *in, size_t inLen, uint8_t *out,
                       size_t &outLen, encryptionFunction func, int senderId, size_t counter);

void performDecryption(int recieverId, uint8_t *encryptedKeyAndData,
                       size_t encryptedKeyAndDataLen, uint8_t *data,
                       size_t &dataLen, encryptionFunction func, int senderId, size_t counter);

void sign(int senderId, void *data, size_t dataLen, void *signature,
          size_t &signatureLen, encryptionFunction func);

void verify(int senderId, void *data, size_t dataLen, void *signature,
            size_t signatureLen, encryptionFunction func);

uint8_t *extractFromPointer(const void *in, int length);

size_t getSymmEncLen(size_t inLen);

size_t getEncryptedLen(int senderId, size_t inLen);

size_t getDecryptedLen(int senderId, size_t inLen);

AESKeyLength mapToAESKeyLength(encryptionFunction func);

size_t convertAESKeyLengthToInt(AESKeyLength keyLength);

size_t getAsymmKeyBitsEnc(encryptionFunction func);

size_t getAsymmKeyBitsSign(encryptionFunction func);

size_t getAsymmEncLen(encryptionFunction func);

size_t getAsymmSignLen(encryptionFunction func);

void printBufferHex(const uint8_t *buffer, size_t len, std::string message);


////////////////////////////////////////////////////////////////////////////////////////


void ECCencrypt(int senderId, int receiverId, void *in, size_t inLen, void *out,
            size_t &outLen);
            
void ECCdecrypt(int senderId, int receiverId, void *in, size_t inLen, void *out,
            size_t &outLen);

#endif  // __CRYPTO_API_H__