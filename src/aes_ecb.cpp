#include "../include/aes_stream.h"
#include <thread>
#include <vector>
#include <cstring>
#include "../include/aes_stream.h"

/**
 * @brief Encrypts a single block of data using AES in ECB mode.
 * 
 * This function wraps the `encryptBlock` function to allow it to be used
 * in a threaded context. It performs AES encryption on a single block of
 * plaintext using the provided round keys and key length.
 * 
 * @param in Pointer to the input block of plaintext data (BLOCK_BYTES_LEN bytes).
 * @param out Pointer to the output block where the encrypted ciphertext will be stored (BLOCK_BYTES_LEN bytes).
 * @param roundKeys Pointer to the array of round keys used for AES encryption.
 * @param keyLength The length of the AES key being used (e.g., AES_KEY_LENGTH_128, AES_KEY_LENGTH_192, AES_KEY_LENGTH_256).
 */
void encryptBlockThreadedECB(const unsigned char in[], unsigned char out[],
                             unsigned char *roundKeys, AESKeyLength keyLength)
{
    encryptBlock(in, out, roundKeys, keyLength);
}

/**
 * @brief Encrypts multiple blocks of data using AES in ECB mode with multithreading.
 * 
 * This function divides the input plaintext into blocks and encrypts each block
 * in parallel using multiple threads. The function ensures that each block is
 * encrypted using AES in ECB mode, and all threads are joined before completing.
 * 
 * @param plaintext Pointer to the input plaintext data to be encrypted.
 * @param ciphertext Pointer to the buffer where the encrypted ciphertext will be stored.
 * @param length The total length of the plaintext data (must be a multiple of BLOCK_BYTES_LEN).
 * @param roundKeys Pointer to the array of round keys used for AES encryption.
 * @param keyLength The length of the AES key being used (e.g., AES_KEY_LENGTH_128, AES_KEY_LENGTH_192, AES_KEY_LENGTH_256).
 */
void encryptECBMultithreaded(const unsigned char *plaintext,
                             unsigned char *ciphertext, unsigned int length,
                             unsigned char *roundKeys, AESKeyLength keyLength)
{
    unsigned int numBlocks = length / BLOCK_BYTES_LEN;
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < numBlocks; ++i) {
        threads.push_back(std::thread(
            encryptBlockThreadedECB, &plaintext[i * BLOCK_BYTES_LEN],
            &ciphertext[i * BLOCK_BYTES_LEN], roundKeys, keyLength));
    }

    for (auto &th : threads) {
        th.join();
    }
}

/**
 * @brief Decrypts a single block of data using AES in ECB mode.
 * 
 * This function wraps the `decryptBlock` function to allow it to be used
 * in a threaded context. It performs AES decryption on a single block of
 * ciphertext using the provided round keys and key length.
 * 
 * @param input Pointer to the input block of ciphertext data (BLOCK_BYTES_LEN bytes).
 * @param output Pointer to the output block where the decrypted plaintext will be stored (BLOCK_BYTES_LEN bytes).
 * @param roundKeys Pointer to the array of round keys used for AES decryption.
 * @param keyLength The length of the AES key being used (e.g., AES_KEY_LENGTH_128, AES_KEY_LENGTH_192, AES_KEY_LENGTH_256).
 */
void decryptBlockThreadedECB(const unsigned char *input, unsigned char *output,
                             unsigned char *roundKeys, AESKeyLength keyLength)
{
    decryptBlock(input, output, roundKeys, keyLength);
}

/**
 * @brief Decrypts multiple blocks of data using AES in ECB mode with multithreading.
 * 
 * This function divides the input ciphertext into blocks and decrypts each block
 * in parallel using multiple threads in ECB (Electronic Codebook) mode. The function
 * ensures that each block is decrypted using AES with the provided round keys, and
 * all threads are joined before completing.
 * 
 * @param ciphertext Pointer to the input ciphertext data to be decrypted.
 * @param plaintext Pointer to the buffer where the decrypted plaintext will be stored.
 * @param length The total length of the ciphertext data (must be a multiple of BLOCK_BYTES_LEN).
 * @param roundKeys Pointer to the array of round keys used for AES decryption.
 * @param keyLength The length of the AES key being used (e.g., AES_KEY_LENGTH_128, AES_KEY_LENGTH_192, AES_KEY_LENGTH_256).
 */
void decryptECBMultithreaded(const unsigned char *ciphertext,
                             unsigned char *plaintext, unsigned int length,
                             unsigned char *roundKeys, AESKeyLength keyLength)
{
    unsigned int numBlocks = length / BLOCK_BYTES_LEN;
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < numBlocks; i++)
        threads.push_back(std::thread(
            decryptBlockThreadedECB, &ciphertext[i * BLOCK_BYTES_LEN],
            &plaintext[i * BLOCK_BYTES_LEN], roundKeys, keyLength));

    for (auto &th : threads)
        th.join();
}

/**
 * @brief Starts AES encryption in ECB mode.
 * 
 * This function initializes the encryption process by encrypting the first block of data
 * and storing the encryption key for further use.
 * 
 * @param block The input plaintext block to encrypt.
 * @param inLen The length of the input data.
 * @param out The buffer to store the encrypted output.
 * @param outLen The length of the encrypted output.
 * @param key The encryption key.
 * @param keyLength The length of the AES key being used (e.g., AES_KEY_LENGTH_128, AES_KEY_LENGTH_192, AES_KEY_LENGTH_256).
 */
void AESEcb::encryptStart(unsigned char block[], unsigned int inLen,
                          unsigned char *out, unsigned int outLen,
                          unsigned char *key, AESKeyLength keyLength)
{
    encrypt(block, inLen, key, out, outLen, nullptr, nullptr, keyLength);
    this->key = new unsigned char[keyLength];
    memcpy(this->key, key, keyLength);
    this->keyLength = keyLength;
}

/**
 * @brief Continues AES encryption in ECB mode.
 * 
 * This function continues the encryption process for additional blocks
 * after the encryption has been started with `encryptStart`.
 * 
 * @param block The input plaintext block to encrypt.
 * @param inLen The length of the input data.
 * @param out The buffer to store the encrypted output.
 * @param outLen The length of the encrypted output.
 */
void AESEcb::encryptContinue(unsigned char block[], unsigned int inLen,
                             unsigned char *out, unsigned int outLen)
{
    encrypt(block, inLen, key, out, outLen, nullptr, nullptr, keyLength);
}

/**
 * @brief Starts AES decryption in ECB mode.
 * 
 * This function initializes the decryption process by decrypting the first block of data
 * and storing the decryption key for further use.
 * 
 * @param block The input ciphertext block to decrypt.
 * @param inLen The length of the input data.
 * @param out The buffer to store the decrypted output.
 * @param outLen The length of the decrypted output.
 * @param key The decryption key.
 * @param keyLength The length of the AES key being used (e.g., AES_KEY_LENGTH_128, AES_KEY_LENGTH_192, AES_KEY_LENGTH_256).
 */
void AESEcb::decryptStart(unsigned char block[], unsigned int inLen,
                          unsigned char *out, unsigned int &outLen,
                          unsigned char *key, AESKeyLength keyLength)
{
    decrypt(block, inLen, key, out, outLen, nullptr, nullptr, keyLength);
}

/**
 * @brief Continues AES decryption in ECB mode.
 * 
 * This function continues the decryption process for additional blocks
 * after the decryption has been started with `decryptStart`.
 * 
 * @param block The input ciphertext block to decrypt.
 * @param inLen The length of the input data.
 * @param out The buffer to store the decrypted output.
 * @param outLen The length of the decrypted output.
 */
void AESEcb::decryptContinue(unsigned char block[], unsigned int inLen,
                             unsigned char *out, unsigned int &outLen)
{
    decrypt(block, inLen, key, out, outLen, nullptr, nullptr, keyLength);
}

/**
 * @brief Encrypts data using AES in ECB mode.
 * 
 * This function performs AES encryption on the input data, applying necessary padding.
 * It uses multithreading to encrypt each block of data in parallel.
 * 
 * @param in The input plaintext data.
 * @param inLen The length of the input data.
 * @param key The encryption key.
 * @param out The buffer to store the encrypted output.
 * @param outLen The length of the encrypted output.
 * @param iv Unused in ECB mode.
 * @param lastData Unused in ECB mode.
 * @param keyLength The length of the AES key being used (e.g., AES_KEY_LENGTH_128, AES_KEY_LENGTH_192, AES_KEY_LENGTH_256).
 */
void AESEcb::encrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                     unsigned char *out, unsigned int outLen,
                     const unsigned char *iv, unsigned char *lastData,
                     AESKeyLength keyLength)
{
    size_t paddedLength = getPaddedLength(inLen);
    unsigned char *paddedIn = new unsigned char[paddedLength];
    padMessage(in, inLen, paddedIn);
    unsigned char block[BLOCK_BYTES_LEN];
    memcpy(block, paddedIn, BLOCK_BYTES_LEN);
    encryptECBMultithreaded(paddedIn, out, paddedLength, key, keyLength);
}

void AESEcb::decrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                     unsigned char *out, unsigned int &outLen,
                     const unsigned char *iv, unsigned char *lastData,
                     AESKeyLength keyLength)
{
    checkLength(inLen);
    outLen = inLen;
    unsigned char *roundKeys =
        new unsigned char[(aesKeyLengthData[keyLength].numRound + 1) *
                          NUM_BLOCKS * 4];
    keyExpansion(key, roundKeys, keyLength);
    for (unsigned int i = 0; i < outLen; i += BLOCK_BYTES_LEN)
        decryptECBMultithreaded(in, out, outLen, roundKeys, keyLength);
    outLen = getUnpadMessageLength(out, inLen);
    delete[] roundKeys;
}