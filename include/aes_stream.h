#include "aes.h"
#include "return_codes.h"
#include <functional>
#include <map>
#include <string>

class StreamAES 
{
  public:

    unsigned char* iv;
    unsigned char* lastBlock;
    AESKeyLength keyLength;
    unsigned char* key;
    unsigned char* lastData;
 
    StreamAES():iv(new unsigned char[16]),lastBlock(new unsigned char[16]){};
    /** 
     @brief Encrypts the initial block of data using AES in CBC mode.
 
     This function generates a random initialization vector (IV) and then 
     encrypts the given block of data. The encrypted data and the IV are 
     concatenated and stored in the output buffer.
 
     @param block Input data block to encrypt.
     @param inLen Length of the input data block.
     @param[out] out Pointer to the output buffer where encrypted data will be stored.
     @param[out] outLen Length of the encrypted output data.
     @param key Encryption key.
     @param keyLength Length of the AES key (128, 192, or 256 bits).
     */
    virtual CK_RV encryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen,unsigned char* key, AESKeyLength keyLength) = 0;

    /**
     @brief Continues encryption of data using AES in CBC mode.

     This function continues the encryption process for the given block of data 
     using the last encrypted block as the IV.

     @param block Input data block to encrypt.
     @param inLen Length of the input data block.
     @param[out] out Pointer to the output buffer where encrypted data will be stored.
     @param[out] outLen Length of the encrypted output data.
    */
    virtual CK_RV encryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outlen) = 0;

    /**
     @brief Decrypts the initial block of data using AES in CBC mode.

     This function extracts the initialization vector (IV) from the end of the 
     input data block and then decrypts the given block of data. The decrypted 
     data is stored in the output buffer.

     @param block Encrypted input data block to decrypt.
     @param inLen Length of the encrypted input data block.
     @param[out] out Pointer to the output buffer where decrypted data will be stored.
     @param[out] outLen Length of the decrypted output data.
     @param key Decryption key.
     @param keyLength Length of the AES key (128, 192, or 256 bits).
    */
    virtual CK_RV decryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outlen,unsigned char* key, AESKeyLength keyLength) = 0;

    /**
     @brief Continues decryption of data using AES in CBC mode.

     This function continues the decryption process for the given block of data 
     using the last decrypted block as the IV.

     @param block Encrypted input data block to decrypt.
     @param inLen Length of the encrypted input data block.
     @param[out] out Pointer to the output buffer where decrypted data will be stored.
     @param[out] outLen Length of the decrypted output data.
    */
    virtual CK_RV decryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outlen) = 0;

    /**
     Decrypts data using AES in CBC mode. 
     @param in Encrypted input data.
     @param inLen Length of input data.
     @param key Decryption key.
     @param[out] out Decrypted output data.
     @param[out] outLen Length of decrypted data.
     @param iv Initialization vector.
     @param keyLength AES key length (128, 192, 256 bits).
    */
    virtual CK_RV decrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                    unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength)  = 0;

    /**
     Encrypts data using AES.
     @param in Input data.
     @param inLen Length of input data.
     @param key Encryption key.
     @param[out] out Encrypted output data.
     @param[out] outLen Length of encrypted data.
     @param iv Initialization vector.
     @param keyLength AES key length (128, 192, 256 bits).
    */
    virtual CK_RV encrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                    unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength)  = 0;

};

class AESEcb:public StreamAES
{
    CK_RV encryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen,unsigned char* key, AESKeyLength keyLength) override;
    CK_RV encryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outlen) override;
    CK_RV decryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outlen,unsigned char* key, AESKeyLength keyLength) override;
    CK_RV decryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outlen) override;
    CK_RV decrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                    unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength) override;
    CK_RV encrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                    unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength) override;
};

class AESCbc:public StreamAES
{
    CK_RV encryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen,unsigned char* key, AESKeyLength keyLength) override;
    CK_RV encryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outlen) override;
    CK_RV decryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outlen,unsigned char* key, AESKeyLength keyLength) override;
    CK_RV decryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outlen) override;
    CK_RV decrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                    unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength) override;
    CK_RV encrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                    unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength) override;
};

class AESCfb:public StreamAES
{
    CK_RV encryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen,unsigned char* key, AESKeyLength keyLength) override;
    CK_RV encryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outlen) override;
    CK_RV decryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outlen,unsigned char* key, AESKeyLength keyLength) override;
    CK_RV decryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outlen) override;
    CK_RV decrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                    unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength) override;
    CK_RV encrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                    unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength) override;
};

class AESOfb:public StreamAES
{
    CK_RV encryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen,unsigned char* key, AESKeyLength keyLength) override;
    CK_RV encryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outlen) override;
    CK_RV decryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outlen,unsigned char* key, AESKeyLength keyLength) override;
    CK_RV decryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outlen) override;
    CK_RV decrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                    unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength) override;
    CK_RV encrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                    unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength) override;
};

class AESCtr:public StreamAES
{
    CK_RV encryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen,unsigned char* key, AESKeyLength keyLength) override;
    CK_RV encryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outlen) override;
    CK_RV decryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outlen,unsigned char* key, AESKeyLength keyLength) override;
    CK_RV decryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outlen) override;
    CK_RV decrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                    unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength) override;
    CK_RV encrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                    unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength) override;
};


