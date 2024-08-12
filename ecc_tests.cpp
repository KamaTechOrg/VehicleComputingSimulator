#include <gmpxx.h>
#include "gtest/gtest.h"
#include <string>
#include "src/ecc.h"

// This test case checks the encryption and decryption functionality of the ECC class.
TEST(ECCTest, EncryptDecrypt)
{
    mpz_class privateKey = generatePrivateKey();
    Point publicKey = generatePublicKey(privateKey);
    // The message to be encrypted
    std::string message = "hello";

    // Encrypt the message
    auto cipher = encryptECC(message, publicKey);

    // Decrypt the message
   auto decryptedMessage = decryptECC(cipher, privateKey);

    // Check if the decrypted message matches the original message
    EXPECT_EQ(message, decryptedMessage);
}

TEST(ECCTest,sign)
{
    std::vector<uint8_t> hash = {0xff,0xef,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
    0xff,0xef,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
    mpz_class privateKey = generatePrivateKey();
    Point publicKey = generatePublicKey(privateKey);
    std::pair<mpz_class,mpz_class> sign = signMessageECC(hash, privateKey);
    EXPECT_TRUE(verifySignatureECC(hash,  sign.first, sign.second,publicKey));
}
// The main function for running all the tests
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}