#ifndef __HSM_H__
#define __HSM_H__

#include <string>
#include <map>
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
    ENCRYPT_256_AND_SIGN_SHA256_CBC,
    ENCRYPT_256_AND_SIGN_SHA256_CFB,
    ENCRYPT_256_AND_SIGN_SHA256_CTR,
    ENCRYPT_256_AND_SIGN_SHA256_ECB,
    ENCRYPT_256_AND_SIGN_SHA256_OFB
};

enum hashFunction{
    SHA_256,
    SHA3_512
};

class Hsm {
   public:
    static encryptionFunction getEncryptionFunctionType(int id);
    static void addProccessKeyandFuncType(int keyID, encryptionFunction func);
    static void setKey(int keyId, AsymmetricKeys *keys);
    static AsymmetricKeys *getKey(int keyId);
    static bool isRsaEncryption(encryptionFunction func);

   private:
    static std::map<int, AsymmetricKeys *> keys;
    static std::map<int, encryptionFunction> encryptionFunctions;
};

#endif  // __HSM_H__