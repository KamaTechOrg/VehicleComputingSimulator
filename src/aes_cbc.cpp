#include "../include/aes_stream.h"
#include <cstring>

void AESCbc::encryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen,unsigned char* key, AESKeyLength keyLength)
{
    generateRandomIV(iv);
    encrypt(block, inLen, key, out, outLen, iv,nullptr, keyLength);
    //unsigned char *newOut = new unsigned char[outLen + BLOCK_BYTES_LEN];
    //memcpy(newOut, out, outLen);
    memcpy(out + outLen, iv, BLOCK_BYTES_LEN);
    memcpy(lastBlock, out, outLen);
    //out = newOut;
    this -> key = key;
    this -> keyLength = keyLength;

    outLen += BLOCK_BYTES_LEN;
}

void AESCbc::encryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int &outLen)
{
  encrypt(block, inLen, key,out, outLen, lastBlock, nullptr, keyLength);
}

void AESCbc::decryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int &outLen,unsigned char* key, AESKeyLength keyLength)
{
  this-> iv = block + inLen - BLOCK_BYTES_LEN;
  decrypt(block,  inLen - BLOCK_BYTES_LEN, key, out, outLen, block + inLen - BLOCK_BYTES_LEN, nullptr, keyLength);
  memcpy(lastBlock, out, outLen - BLOCK_BYTES_LEN);
}

void AESCbc::decryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen)
{
  decrypt(block,  inLen , key, out, outLen, lastBlock,nullptr, keyLength);
}

void AESCbc::encrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                    unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength)
{
    padMessage(in, inLen, outLen);
    unsigned char block[BLOCK_BYTES_LEN];
    //out = new unsigned char[outLen];
    unsigned char *roundKeys =
        new unsigned char[(aesKeyLengthData[keyLength].numRound + 1) * NUM_BLOCKS * 4];
    keyExpansion(key, roundKeys, keyLength);
    memcpy(block, iv, BLOCK_BYTES_LEN);
    for (unsigned int i = 0; i < outLen; i += BLOCK_BYTES_LEN) {
        xorBlocks(block, in + i, block, BLOCK_BYTES_LEN);
        encryptBlock(block, out + i, roundKeys, keyLength);
        memcpy(block, out + i, BLOCK_BYTES_LEN);
    }
    delete[] roundKeys;
}

void AESCbc::decrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
             unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength)
{
    checkLength(inLen);
    unsigned char block[BLOCK_BYTES_LEN];
    outLen = inLen;
    out = new unsigned char[outLen];
    unsigned char *roundKeys =
        new unsigned char[(aesKeyLengthData[keyLength].numRound + 1) * NUM_BLOCKS * 4];
    keyExpansion(key, roundKeys, keyLength);
    memcpy(block, iv, BLOCK_BYTES_LEN);
    for (unsigned int i = 0; i < outLen; i += BLOCK_BYTES_LEN) {
        decryptBlock(in + i, out + i, roundKeys, keyLength);
        xorBlocks(block, out + i, out + i, BLOCK_BYTES_LEN);
        memcpy(block, in + i, BLOCK_BYTES_LEN);
    }
    unpadMessage(out, outLen);
    delete[] roundKeys;
}