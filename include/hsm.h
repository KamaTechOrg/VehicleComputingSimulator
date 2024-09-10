#ifndef __HSM_H__
#define __HSM_H__

#include <string>
#include <map>
#include <gmpxx.h>
#include "asymmetric_keys.h"

#define rsaKeySize 1024

enum encryptionFunction {
    SIGNATURE_RSA,
    SIGNATURE_ECC,
    ENCRYPT_128_AND_SIGN_RSA,
    ENCRYPT_192_AND_SIGN_RSA,
    ENCRYPT_256_AND_SIGN_RSA,
    ENCRYPT_128_AND_SIGN_ECC,
    ENCRYPT_192_AND_SIGN_ECC,
    ENCRYPT_256_AND_SIGN_ECC
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