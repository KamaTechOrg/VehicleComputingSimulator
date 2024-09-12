#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include "rsa.h"
#include "logger.h"
#include "prime_tests.h"
using namespace std;
#define PADDING_MIN_LEN 11
#define PRIME_TEST_ROUNDS 40

bool allowedKeySizes(size_t keySize)
{
    return keySize == 1024 || keySize == 2048 || keySize == 4096;
}

void rsaGeneratePrime(size_t bits, BigInt64 &prime)
{
    BigInt64 random = BigInt64(BigInt64::CreateModes::RANDOM, bits);
    prime = nextPrimeDivideToChunks(random, PRIME_TEST_ROUNDS);
}

void rsaKeysGeneration(const BigInt64 &p, const BigInt64 &q, size_t keySize,
                       BigInt64 &e, BigInt64 &d)
{
    BigInt64 phi;
    BigInt64 gcdVal;

    // phi(n) = (p-1) * (q-1)
    BigInt64 pMinus1 = p - 1;
    BigInt64 qMinus1 = q - 1;
    phi = pMinus1 * qMinus1;

    // Choose e such that 1 < e < phi and gcd(e, phi) = 1 (coprime)
    // A common choice for e is 65537
    e = 65537;
    gcdVal = gcd(e, phi);
    while (gcdVal != 1) {
        e += 2;
        if (e > phi)
            e = 17;
    }

    // d is the modular inverse of e modulo ϕ(n) or e^-1 mod phi
    //  0<d<phi and  (d * e) % phi = 1 ,<- (d*e -1) is divisible by phi
    d = modularInverse(e, phi);
}

CK_RV rsaGenerateKeys(size_t keySize, uint8_t *pubExponent, size_t pubLen,
                      uint8_t *privExponent, size_t privLen, uint8_t *modulus,
                      size_t modulusLen)
{
    logger rsaLogger("RSA encryption");
    rsaLogger.logMessage(logger::LogLevel::INFO, "generation RSA key pair of " +
                                                     to_string(keySize) +
                                                     " bits : started...");
    if (!allowedKeySizes(keySize))
        return CKR_KEY_SIZE_RANGE;  
    if (!pubLen == rsaGetPublicExponentLen() ||
        !privLen == rsaGetPrivateExponent(keySize) ||
        !modulusLen == rsaGetModulusLen(keySize))
        return CKR_BUFFER_TOO_SMALL;
    // Generate prime numbers for public and
    // private keys
    BigInt64 p, q, n, e, d;
    try {
        // Generate large prime numbers
        rsaGeneratePrime(keySize / 2, p);
        rsaGeneratePrime(keySize / 2, q);

        n = p * q;

        rsaKeysGeneration(p, q, keySize, e, d);
    }
    catch (const std::exception &e) {
        rsaLogger.logMessage(logger::LogLevel::ERROR,
                             "generation RSA key pair : failed");
        return CKR_KEY_FUNCTION_NOT_PERMITTED;
    }
    memset(privExponent, 0, privLen);
    n.exportTo(modulus, modulusLen, BigInt64::CreateModes::BIG_ENDIANESS);
    e.exportTo(pubExponent, pubLen, BigInt64::CreateModes::BIG_ENDIANESS);
    d.exportTo(privExponent, privLen, BigInt64::CreateModes::BIG_ENDIANESS);
    rsaLogger.logMessage(logger::LogLevel::INFO,
                         "generation RSA key pair : success");
    return CKR_OK;
}

void rsaPkcs1v15Pad(const uint8_t *plaintext, size_t plaintextLen,
                    uint8_t *padded, size_t keySize)
{
    size_t paddingLen = keySize - plaintextLen - 3;  // Padding length
    if (paddingLen < 8) {                            // Minimum padding length
        throw std::runtime_error("Plaintext is too long for padding");
    }

    // Start with 0x00 0x02
    padded[0] = 0x00;
    padded[1] = 0x02;

    // Add non-zero random padding
    for (size_t i = 2; i < paddingLen + 2; i++) {
        uint8_t rnd = static_cast<uint8_t>(rand() % 256);
        while (rnd == 0) {
            rnd = static_cast<uint8_t>(rand() % 256);
        }
        padded[i] = rnd;
    }

    // Add 0x00 separator
    padded[paddingLen + 2] = 0x00;

    // Copy the plaintext
    std::memcpy(padded + paddingLen + 3, plaintext, plaintextLen);
}
void rsaPkcs1v15Unpad(const uint8_t *padded, size_t paddedLen,
                      uint8_t *plaintext, size_t *plaintextLen)
{
    if (paddedLen < PADDING_MIN_LEN || padded[0] != 0x00 || padded[1] != 0x02) {
        throw std::runtime_error("Invalid padding");
    }

    // Find 0x00 separator
    size_t i = 2;
    while (i < paddedLen && padded[i] != 0x00) {
        ++i;
    }

    if (i == paddedLen) {
        throw std::runtime_error("Invalid padding: No separator found");
    }

    // Copy plaintext
    *plaintextLen = paddedLen - i - 1;
    std::memcpy(plaintext, padded + i + 1, *plaintextLen);
}
CK_RV rsaEncrypt(const uint8_t *plaintext, size_t plaintextLen,
                 const uint8_t *modulus, size_t modulusLen,
                 const uint8_t *exponent, size_t exponentLen,
                 uint8_t *ciphertext, size_t ciphertextLen, size_t keySize)
{
    if (!allowedKeySizes(keySize))
        return CKR_KEY_SIZE_RANGE;  // invalid key size
    if ((!exponentLen == rsaGetPublicExponentLen() &&
         !exponentLen == rsaGetPrivateExponent(keySize)) ||
        !modulusLen == rsaGetModulusLen(keySize))
        return CKR_BUFFER_TOO_SMALL;
    logger rsaLogger("RSA encryption");
    rsaLogger.logMessage(logger::LogLevel::INFO, "encrypting with RSA");

    size_t keySizeBytes = keySize / BITS_IN_BYTE;

    if (plaintextLen > rsaGetPlainMaxLen(keySize))
        return 6;
    if (ciphertextLen != keySizeBytes)
        return 6;

    size_t paddedLen = keySizeBytes;
    // Padding plaintext to keySizeBytes
    uint8_t *padded = new uint8_t[keySizeBytes];
    rsaPkcs1v15Pad(plaintext, plaintextLen, padded, keySizeBytes);

    // Convert padded plaintext to BigInt64
    BigInt64 plainNumber(padded, paddedLen,
                         BigInt64::CreateModes::BIG_ENDIANESS);
    BigInt64 n(modulus, modulusLen, BigInt64::CreateModes::BIG_ENDIANESS);
    BigInt64 key(exponent, exponentLen, BigInt64::CreateModes::BIG_ENDIANESS);
    // Encrypt message:
    BigInt64 cipherNumber = modularExponentiation(plainNumber, key, n);

    memset(ciphertext, 0, keySizeBytes);

    cipherNumber.exportTo(ciphertext, ciphertextLen,
                          BigInt64::CreateModes::BIG_ENDIANESS);

    delete[] padded;
    return CKR_OK;
}

CK_RV rsaDecrypt(const uint8_t *ciphertext, size_t ciphertextLen,
                 const uint8_t *modulus, size_t modulusLen,
                 const uint8_t *exponent, size_t exponentLen,
                 uint8_t *plaintext, size_t *plaintextLen, size_t keySize)
{
    if (!allowedKeySizes(keySize))
        return CKR_KEY_SIZE_RANGE;  
    if ((!exponentLen == rsaGetPublicExponentLen() &&
         !exponentLen == rsaGetPrivateExponent(keySize)) ||
        !modulusLen == rsaGetModulusLen(keySize))
        return CKR_BUFFER_TOO_SMALL;
    size_t keySizeBytes = keySize / 8;

    if (ciphertextLen != keySizeBytes)
        return CKR_BUFFER_TOO_SMALL;
    logger rsaLogger("RSA encryption");
    rsaLogger.logMessage(logger::LogLevel::INFO, "decryption with RSA");
    // Convert ciphertext to BigInt64
    BigInt64 cipherNumber(ciphertext, ciphertextLen,
                          BigInt64::CreateModes::BIG_ENDIANESS);
    BigInt64 n(modulus, modulusLen, BigInt64::CreateModes::BIG_ENDIANESS);
    BigInt64 key(exponent, exponentLen, BigInt64::CreateModes::BIG_ENDIANESS);
    // Decrypt message: m = cipher^d % n
    BigInt64 plainNumber = modularExponentiation(cipherNumber, key, n);

    uint8_t *padded = new uint8_t[keySizeBytes];
    size_t paddedLen = keySizeBytes;

    plainNumber.exportTo(padded, paddedLen,
                         BigInt64::CreateModes::BIG_ENDIANESS);

    // Remove padding
    try {
        rsaPkcs1v15Unpad(padded, paddedLen, plaintext, plaintextLen);
    }
    catch (std::exception &e) {
        return CKR_ENCRYPTED_DATA_INVALID;
    }
    delete[] padded;
    return CKR_OK;
}

size_t rsaGetEncryptedLen(size_t keySize)
{
    return keySize / 8;
}

size_t rsaGetPlainMaxLen(size_t keySize)
{
    return keySize / 8 - PADDING_MIN_LEN;
}

size_t rsaGetDecryptedLen(size_t keySize)
{
    // Minimum padding length for PKCS#1 v1.5 is 11 bytes
    // Remove the padding: The maximum length of the plaintext is keySize -
    // minPaddingLength
    return keySize / BITS_IN_BYTE - PADDING_MIN_LEN;
}

size_t rsaGetModulusLen(size_t keySize)
{
    return keySize / BITS_IN_BYTE;
}

size_t rsaGetPublicExponentLen()
{
    return BYTES_IN_LIMB;
}

size_t rsaGetPrivateExponent(size_t keySize)
{
    return keySize / BITS_IN_BYTE;
}
