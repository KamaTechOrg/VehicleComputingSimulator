#include "../include/aes_stream.h"

CK_RV AESOfb::encryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen,unsigned char* key, AESKeyLength keyLength) 
{
    unsigned char* lastData = new unsigned char[BLOCK_BYTES_LEN];
    if(!lastData)
        return CKR_FUNCTION_FAILED;
    generateRandomIV(iv);
    memcpy(lastData, iv, BLOCK_BYTES_LEN);
    encrypt(block, inLen, key, out, outLen, iv,lastData, keyLength);
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
    this-> lastData = lastData;
    outLen += BLOCK_BYTES_LEN;

    return CKR_OK;
}

CK_RV AESOfb::encryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int &outLen)
{
    return encrypt(block, inLen, key,out, outLen, lastBlock, lastData, keyLength);
}

CK_RV AESOfb::decryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int &outLen,unsigned char* key, AESKeyLength keyLength)
{
    unsigned char* lastData = new unsigned char[BLOCK_BYTES_LEN];
  memcpy(lastData, iv, BLOCK_BYTES_LEN);
  this-> iv = block + inLen - BLOCK_BYTES_LEN;
  CK_RV status = decrypt(block,  inLen - BLOCK_BYTES_LEN, key, out, outLen, block + inLen - BLOCK_BYTES_LEN, lastData, keyLength);
  this-> lastBlock = out;
  this->lastData = lastData;

  return status;
}

CK_RV AESOfb::decryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen)
{
   return decrypt(block,  inLen , key, out, outLen, lastBlock, lastData, keyLength);
}

CK_RV AESOfb::encrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
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
    memcpy(feedback, lastData, BLOCK_BYTES_LEN);
    for (unsigned int i = 0; i < outLen; i += BLOCK_BYTES_LEN) {
        encryptBlock(feedback, block, roundKeys, keyLength);
        xorBlocks(in + i, block, out+ i,BLOCK_BYTES_LEN);
        for (unsigned int j = 0; j < BLOCK_BYTES_LEN; ++j) 
            out[i + j] = in[i + j] ^ block[j];
        memcpy(feedback, block, BLOCK_BYTES_LEN);
        memcpy(lastData, feedback, BLOCK_BYTES_LEN);
    }
    delete[] roundKeys;

    return CKR_FUNCTION_FAILED;
}

CK_RV AESOfb::decrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength) 
{
    if(!checkLength(inLen))
        return CKR_FUNCTION_FAILED;
    unsigned char block[BLOCK_BYTES_LEN];
    outLen = inLen;
    out = new unsigned char[outLen];
    unsigned char feedback[BLOCK_BYTES_LEN];
    unsigned char *roundKeys = new unsigned char[(aesKeyLengthData[keyLength].numRound + 1) * NUM_BLOCKS * 4];
    keyExpansion(key, roundKeys, keyLength);
    memcpy(feedback, lastData, BLOCK_BYTES_LEN);
        for (unsigned int i = 0; i < outLen; i += BLOCK_BYTES_LEN) {
            encryptBlock(feedback, block, roundKeys, keyLength);
            xorBlocks(in + i, block, out + i, BLOCK_BYTES_LEN);
            memcpy(feedback, block, BLOCK_BYTES_LEN);
            memcpy(lastData, feedback, BLOCK_BYTES_LEN);
    }
    unpadMessage(out, outLen);
    delete[] roundKeys;

    return CKR_FUNCTION_FAILED;
}