#include "../include/aes_stream.h"

CK_RV AESCfb::encryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen,unsigned char* key, AESKeyLength keyLength)
{
    generateRandomIV(iv);
    CK_RV status = encrypt(block, inLen, key, out, outLen, iv,nullptr, keyLength);
    unsigned char *newOut = new unsigned char[outLen + BLOCK_BYTES_LEN];
    if(!newOut)
      return CKR_FUNCTION_FAILED;
    memcpy(newOut, out, outLen);
    memcpy(newOut + outLen, iv, BLOCK_BYTES_LEN);
    memcpy(lastBlock, out, outLen);
    out = newOut;
    this -> lastBlock  = out;
    this -> key = key;
    this -> keyLength = keyLength;
    outLen += BLOCK_BYTES_LEN;

    return status;
}

CK_RV AESCfb::encryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int &outLen)
{
  return encrypt(block, inLen, key,out, outLen, lastBlock, nullptr, keyLength);
}

CK_RV AESCfb::decryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int &outLen,unsigned char* key, AESKeyLength keyLength)
{
  this-> iv = block + inLen - BLOCK_BYTES_LEN;
  CK_RV status = decrypt(block,  inLen - BLOCK_BYTES_LEN, key, out, outLen, block + inLen - BLOCK_BYTES_LEN, nullptr, keyLength);
  memcpy(lastBlock, out, outLen - BLOCK_BYTES_LEN);

  return status;
}

CK_RV AESCfb::decryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen)
{
  return decrypt(block,  inLen , key, out, outLen, lastBlock,nullptr, keyLength);
}

CK_RV AESCfb::encrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength) 
{
    padMessage(in, inLen, outLen);
    out = new unsigned char[outLen];
    unsigned char block[BLOCK_BYTES_LEN];
    unsigned char feedback[BLOCK_BYTES_LEN];
    unsigned char *roundKeys = new unsigned char[(aesKeyLengthData[keyLength].numRound + 1) * NUM_BLOCKS * 4];
    if(!roundKeys || !out)
        return CKR_FUNCTION_FAILED;
    keyExpansion(key, roundKeys, keyLength);
    memcpy(feedback, iv, BLOCK_BYTES_LEN);
    for (unsigned int i = 0; i < outLen; i += BLOCK_BYTES_LEN) {
        encryptBlock(feedback, block, roundKeys, keyLength);
        xorBlocks(in + i, block, out + i, BLOCK_BYTES_LEN);
        memcpy(feedback, out + i, BLOCK_BYTES_LEN);
    }

    return CKR_OK;
}

/**
 Decrypts data using AES in CFB mode. 
 @param in Encrypted input data.
 @param inLen Length of input data.
 @param key Decryption key.
 @param[out] out Decrypted output data.
 @param[out] outLen Length of decrypted data.
 @param iv Initialization vector.
 @param keyLength AES key length (128, 192, 256 bits).
 */
CK_RV AESCfb::decrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength) 
{
    if(!checkLength(inLen))
      return CKR_FUNCTION_FAILED;
    unsigned char block[BLOCK_BYTES_LEN];
    outLen = inLen;
    out = new unsigned char[outLen];
       unsigned char feedback[BLOCK_BYTES_LEN];
    unsigned char *roundKeys = new unsigned char[(aesKeyLengthData[keyLength].numRound + 1) * NUM_BLOCKS * 4];
     if(!roundKeys || !out)
        return CKR_FUNCTION_FAILED;
    keyExpansion(key, roundKeys, keyLength);
    memcpy(feedback, iv, BLOCK_BYTES_LEN);
    for (unsigned int i = 0; i < outLen; i += BLOCK_BYTES_LEN) {
        encryptBlock(feedback, block, roundKeys, keyLength);
        xorBlocks(in + i, block, out + i, BLOCK_BYTES_LEN);
        memcpy(feedback, in + i, BLOCK_BYTES_LEN);
    }
    unpadMessage(out, outLen);
    delete[] roundKeys;


    return CKR_OK;
}