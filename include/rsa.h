#ifndef __RSA_H__
#define __RSA_H__

#include "big_int64.h"
#include "return_codes.h"

CK_RV rsaGenerateKeys(size_t keySize, uint8_t *pubExponent, size_t pubLen,
                      uint8_t *privExponent, size_t privLen, uint8_t *modulus,
                      size_t modulusLen);

CK_RV rsaEncrypt(const uint8_t *plaintext, size_t plaintextLen,
                 const uint8_t *modulus, size_t modulusLen,
                 const uint8_t *exponent, size_t exponentLen,
                 uint8_t *ciphertext, size_t ciphertextLen, size_t keySize);

CK_RV rsaDecrypt(const uint8_t *ciphertext, size_t ciphertextLen,
                 const uint8_t *modulus, size_t modulusLen,
                 const uint8_t *exponent, size_t exponentLen,
                 uint8_t *plaintext, size_t *plaintextLen, size_t keySize);

size_t rsaGetEncryptedLen(size_t keySize);
size_t rsaGetPlainMaxLen(size_t keySize);

size_t rsaGetDecryptedLen(size_t keySize);
size_t rsaGetModulusLen(size_t keySize);
size_t rsaGetPublicExponentLen();
size_t rsaGetPrivateExponent(size_t keySize);

#endif  // __RSA_H__