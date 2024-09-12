#include <iostream>
#include <gtest/gtest.h>
#include "tests_utils.h"
#include "rsa.h"

using namespace std;
TEST(RSATest, EncryptDecrypt_124) {
  const int KEY_SIZE = 1024;


  START_TIMER
  size_t nLen = rsaGetModulusLen(KEY_SIZE);
  size_t pubLen = rsaGetPublicExponentLen();
  size_t priLen = rsaGetPrivateExponent(KEY_SIZE);
  uint8_t *nBuff = new uint8_t[nLen];
  uint8_t *pubBuff = new uint8_t[pubLen];
  uint8_t *priBuff = new uint8_t[priLen];

  rsaGenerateKeys(KEY_SIZE, pubBuff,pubLen,priBuff,priLen,nBuff,nLen);
  END_TIMER("1024 key generation")

  printBufferHexa(nBuff,nLen, "n");
  printBufferHexa(pubBuff,pubLen, "public exponent");
  printBufferHexa(priBuff,priLen, "private exponent");


  // Define data to encrypt
  const uint8_t plaintext[] = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  size_t plaintextLen = sizeof(plaintext);

  // Encrypt the data
  size_t ciphertextLen = rsaGetEncryptedLen(KEY_SIZE);
  uint8_t *ciphertext = new uint8_t[ciphertextLen];
  rsaEncrypt(plaintext, plaintextLen, nBuff,nLen, pubBuff,pubLen, ciphertext, ciphertextLen,
             KEY_SIZE);

  // Decrypt the data
  size_t decryptedLen = rsaGetDecryptedLen(KEY_SIZE);
  uint8_t *decrypted = new uint8_t[decryptedLen];
  rsaDecrypt(ciphertext, ciphertextLen,nBuff,nLen, priBuff,priLen, decrypted, &decryptedLen,
             KEY_SIZE);
  printBufferHexa(decrypted, decryptedLen, "plain after decryption");

  // Check that decrypted data matches the original plaintext
  EXPECT_EQ(plaintextLen, decryptedLen);
  EXPECT_EQ(memcmp(plaintext, decrypted, plaintextLen), 0);

  // Clean up
  delete[] ciphertext;
  delete[] decrypted;
}
TEST(RSATest, EncryptDecrypt_2048) {
  const int KEY_SIZE = 2048;



  START_TIMER

  size_t nLen = rsaGetModulusLen(KEY_SIZE);
  size_t pubLen = rsaGetPublicExponentLen();
  size_t priLen = rsaGetPrivateExponent(KEY_SIZE);
  uint8_t *nBuff = new uint8_t[nLen];
  uint8_t *pubBuff = new uint8_t[pubLen];
  uint8_t *priBuff = new uint8_t[priLen];

  rsaGenerateKeys(KEY_SIZE, pubBuff,pubLen,priBuff,priLen,nBuff,nLen);
  END_TIMER("2048 key generation")

  printBufferHexa(nBuff,nLen, "n");
  printBufferHexa(pubBuff,pubLen, "public exponent");
  printBufferHexa(priBuff,priLen, "private exponent");

  // Define data to encrypt
  const uint8_t plaintext[] = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  size_t plaintextLen = sizeof(plaintext);

  // Encrypt the data
  size_t ciphertextLen = rsaGetEncryptedLen(KEY_SIZE);
  uint8_t *ciphertext = new uint8_t[ciphertextLen];
   rsaEncrypt(plaintext, plaintextLen, nBuff,nLen, pubBuff,pubLen, ciphertext, ciphertextLen,
             KEY_SIZE);

  // Decrypt the data
  size_t decryptedLen = rsaGetDecryptedLen(KEY_SIZE);
  uint8_t *decrypted = new uint8_t[decryptedLen];
  rsaDecrypt(ciphertext, ciphertextLen,nBuff,nLen, priBuff,priLen, decrypted, &decryptedLen,
             KEY_SIZE);
  printBufferHexa(decrypted, decryptedLen, "plain after decryption");

  // Check that decrypted data matches the original plaintext
  EXPECT_EQ(plaintextLen, decryptedLen);
  EXPECT_EQ(memcmp(plaintext, decrypted, plaintextLen), 0);

  // Clean up
  delete[] ciphertext;
  delete[] decrypted;
}

TEST(RSATest, EncryptDecrypt_4096) {
  const int KEY_SIZE = 4096;



  START_TIMER

  size_t nLen = rsaGetModulusLen(KEY_SIZE);
  size_t pubLen = rsaGetPublicExponentLen();
  size_t priLen = rsaGetPrivateExponent(KEY_SIZE);
  uint8_t *nBuff = new uint8_t[nLen];
  uint8_t *pubBuff = new uint8_t[pubLen];
  uint8_t *priBuff = new uint8_t[priLen];

  rsaGenerateKeys(KEY_SIZE, pubBuff,pubLen,priBuff,priLen,nBuff,nLen);
  END_TIMER("4096 key generation")

  printBufferHexa(nBuff,nLen, "n");
  printBufferHexa(pubBuff,pubLen, "public exponent");
  printBufferHexa(priBuff,priLen, "private exponent");

  // Define data to encrypt
  const uint8_t plaintext[] = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  size_t plaintextLen = sizeof(plaintext);

  // Encrypt the data
  size_t ciphertextLen = rsaGetEncryptedLen(KEY_SIZE);
  uint8_t *ciphertext = new uint8_t[ciphertextLen];
  rsaEncrypt(plaintext, plaintextLen, nBuff,nLen, pubBuff,pubLen, ciphertext, ciphertextLen,
             KEY_SIZE);

  // Decrypt the data
  size_t decryptedLen = rsaGetDecryptedLen(KEY_SIZE);
  uint8_t *decrypted = new uint8_t[decryptedLen];
   rsaDecrypt(ciphertext, ciphertextLen,nBuff,nLen, priBuff,priLen, decrypted, &decryptedLen,
             KEY_SIZE);
  printBufferHexa(decrypted, decryptedLen, "plain after decryption");

  // Check that decrypted data matches the original plaintext
  EXPECT_EQ(plaintextLen, decryptedLen);
  EXPECT_EQ(memcmp(plaintext, decrypted, plaintextLen), 0);

  // Clean up
  delete[] ciphertext;
  delete[] decrypted;
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}