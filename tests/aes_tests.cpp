#include <cstring>
#include "gtest/gtest.h"
#include "../include/aes.h"
#include "../include/aes_stream_factory.h"  // Assuming this is where your FactoryManager is defined

/* Helper function to setup encryption and decryption */
void testEncryptionDecryption(AESChainingMode mode, AESKeyLength keyLength) {
    unsigned char plain[BLOCK_BYTES_LEN] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };
    unsigned char plain2[BLOCK_BYTES_LEN] = {
        0x00, 0x10, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x78, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };
     unsigned char plain3[BLOCK_BYTES_LEN] = {
        0x00, 0x10, 0x21, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x78, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };

    // Create a factory instance
    StreamAES* streamAES = FactoryManager::getInstance().create(mode);
    ASSERT_NE(streamAES, nullptr);

    std::unique_ptr<unsigned char[]> key(new unsigned char[aesKeyLengthData[keyLength].keySize]);
    generateKey(key.get(), keyLength);

    std::unique_ptr<unsigned char[]> encrypted(new unsigned char[calculatEncryptedLenAES(BLOCK_BYTES_LEN, true)]);
    std::unique_ptr<unsigned char[]> encrypted2(new unsigned char[calculatEncryptedLenAES(BLOCK_BYTES_LEN, false)]);
    std::unique_ptr<unsigned char[]> encrypted3(new unsigned char[calculatEncryptedLenAES(BLOCK_BYTES_LEN, false)]);

    unsigned int outLenEncrypted = 0;
    unsigned int outLenEncrypted2 = 0;
    unsigned int outLenEncrypted3 = 0;

    // Use raw pointers for output parameters
    unsigned char* encryptedPtr = encrypted.get();
    unsigned char* encrypted2Ptr = encrypted2.get();
    unsigned char* encrypted3Ptr = encrypted3.get();

    streamAES->encryptStart(plain, BLOCK_BYTES_LEN, encryptedPtr, outLenEncrypted, key.get(), keyLength);
    streamAES->encryptContinue(plain2, BLOCK_BYTES_LEN, encrypted2Ptr, outLenEncrypted2);
    streamAES->encryptContinue(plain3, BLOCK_BYTES_LEN, encrypted3Ptr, outLenEncrypted3);

    std::unique_ptr<unsigned char[]> decrypted(new unsigned char[calculatDecryptedLenAES(outLenEncrypted, true)]);
    std::unique_ptr<unsigned char[]> decrypted2(new unsigned char[calculatDecryptedLenAES(outLenEncrypted2, false)]);
    std::unique_ptr<unsigned char[]> decrypted3(new unsigned char[calculatDecryptedLenAES(outLenEncrypted3, false)]);

    unsigned int outLenDecrypted = 0;
    unsigned int outLenDecrypted2 = 0;
    unsigned int outLenDecrypted3 = 0;

    // Use raw pointers for output parameters
    unsigned char* decryptedPtr = decrypted.get();
    unsigned char* decrypted2Ptr = decrypted2.get();
    unsigned char* decrypted3Ptr = decrypted3.get();

    streamAES->decryptStart(encryptedPtr, outLenEncrypted, decryptedPtr, outLenDecrypted, key.get(), keyLength);
    streamAES->decryptContinue(encrypted2Ptr, outLenEncrypted2, decrypted2Ptr, outLenDecrypted2);
    streamAES->decryptContinue(encrypted3Ptr, outLenEncrypted3, decrypted3Ptr, outLenDecrypted3);

    ASSERT_EQ(memcmp(plain, decryptedPtr, BLOCK_BYTES_LEN), 0);
    ASSERT_EQ(memcmp(plain2, decrypted2Ptr, BLOCK_BYTES_LEN), 0);
    ASSERT_EQ(memcmp(plain3, decrypted3Ptr, BLOCK_BYTES_LEN), 0);
}

TEST(KeyLengths, KeyLength128_ECB) 
{
    testEncryptionDecryption(AESChainingMode::ECB, AESKeyLength::AES_128);
}

TEST(KeyLengths, KeyLength128_CBC) 
{
    testEncryptionDecryption(AESChainingMode::CBC, AESKeyLength::AES_128);
}

TEST(KeyLengths, KeyLength128_CFB) 
{
    testEncryptionDecryption(AESChainingMode::CFB, AESKeyLength::AES_128);
}

TEST(KeyLengths, KeyLength128_OFB) 
{
    testEncryptionDecryption(AESChainingMode::OFB, AESKeyLength::AES_128);
}

TEST(KeyLengths, KeyLength128_CTR) 
{
    testEncryptionDecryption(AESChainingMode::CTR, AESKeyLength::AES_128);
}

TEST(KeyLengths, KeyLength192_ECB) 
{
    testEncryptionDecryption(AESChainingMode::ECB, AESKeyLength::AES_192);
}

TEST(KeyLengths, KeyLength192_CBC) 
{
    testEncryptionDecryption(AESChainingMode::CBC, AESKeyLength::AES_192);
}

TEST(KeyLengths, KeyLength192_CFB) 
{
    testEncryptionDecryption(AESChainingMode::CFB, AESKeyLength::AES_192);
}

TEST(KeyLengths, KeyLength192_OFB) 
{
    testEncryptionDecryption(AESChainingMode::OFB, AESKeyLength::AES_192);
}

TEST(KeyLengths, KeyLength192_CTR) 
{
    testEncryptionDecryption(AESChainingMode::CTR, AESKeyLength::AES_192);
}

TEST(KeyLengths, KeyLength256_ECB) 
{
    testEncryptionDecryption(AESChainingMode::ECB, AESKeyLength::AES_256);
}

TEST(KeyLengths, KeyLength256_CBC) 
{
    testEncryptionDecryption(AESChainingMode::CBC, AESKeyLength::AES_256);
}

TEST(KeyLengths, KeyLength256_CFB) 
{
    testEncryptionDecryption(AESChainingMode::CFB, AESKeyLength::AES_256);
}

TEST(KeyLengths, KeyLength256_OFB) 
{
    testEncryptionDecryption(AESChainingMode::OFB, AESKeyLength::AES_256);
}

TEST(KeyLengths, KeyLength256_CTR) 
{
    testEncryptionDecryption(AESChainingMode::CTR, AESKeyLength::AES_256);
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}