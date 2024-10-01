#include "crypto_api.h"
#include "debug_utils.h"
#include "temp_hsm.h"
#include <gtest/gtest.h>

class CryptoAPIFixture : public ::testing::Test {
   protected:
    int user1 = 1;
    int user2 = 2;
    size_t counter = 1;
    std::pair<std::string, std::string> rsaKeyIds;
    std::pair<std::string, std::string> eccKeyIds;
    std::vector<KeyPermission> permissions = {
        KeyPermission::DECRYPT, KeyPermission::ENCRYPT, KeyPermission::SIGN,
        KeyPermission::VERIFY, KeyPermission::EXPORTABLE};
    AESChainingMode chainingMode = AESChainingMode::CBC;  // Change as needed
    AESKeyLength keyLength = AESKeyLength::AES_128;       // Change as needed

    void SetUp() override
    {
        rsaKeyIds = generateRSAKeyPair(1, permissions);
        eccKeyIds = generateECCKeyPair(1, permissions);
    }

    void TearDown() override {}
};

#define RSA_TEST
#define ECC_TEST
#define SIGN_VERIFY_TEST

#pragma region RSA tests

#ifdef RSA_TEST
TEST_F(CryptoAPIFixture, rsaEncryptionDecryption)
{
    // Generate RSA key pair for user1

    uint8_t data[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    size_t dataLen = 6;
    //printBufferHexa(data, dataLen, "rsa plain data");

    size_t encryptedLen = getRSAencryptedLength();
    uint8_t *encrypted = new uint8_t[encryptedLen];

    // RSA encryption: user2 -> user1
    CK_RV rv2 = RSAencrypt(user2, rsaKeyIds.first, data, dataLen, encrypted,
                           encryptedLen);
    EXPECT_EQ(CKR_OK, rv2);

    size_t decryptedLen = getRSAdecryptedLength();
    uint8_t *decrypted = new uint8_t[decryptedLen];

    // RSA decryption: user1
    CK_RV rv3 = RSAdecrypt(user1, rsaKeyIds.second, encrypted, encryptedLen,
                           decrypted, &decryptedLen);
    EXPECT_EQ(CKR_OK, rv3);

    //printBufferHexa(decrypted, decryptedLen, "rsa decrypted");

    EXPECT_EQ(0, memcmp(data, decrypted, dataLen));

    delete[] encrypted;
    delete[] decrypted;
}

TEST_F(CryptoAPIFixture, rsaEncryptionWithNullData)
{
    // Generate RSA key pair for user1

    uint8_t *nullData = nullptr;
    size_t dataLen = 0;

    size_t encryptedLen = getRSAencryptedLength();
    uint8_t *encrypted = new uint8_t[encryptedLen];

    // RSA encryption: user2 -> user1
    CK_RV rv = RSAencrypt(user2, rsaKeyIds.first, nullData, dataLen, encrypted,
                          encryptedLen);
    EXPECT_EQ(CKR_EMPTY_BUFFER, rv)
        << "Expected CKR_EMPTY_BUFFER for null data";

    delete[] encrypted;
}

TEST_F(CryptoAPIFixture, rsaDecryptionWithNullData)
{
    // Generate RSA key pair for user1

    uint8_t data[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    size_t dataLen = 6;
    //printBufferHexa(data, dataLen, "rsa plain data");

    size_t encryptedLen = getRSAencryptedLength();
    uint8_t *encrypted = new uint8_t[encryptedLen];

    // RSA encryption: user2 -> user1
    CK_RV rv2 = RSAencrypt(user2, rsaKeyIds.first, data, dataLen, encrypted,
                           encryptedLen);
    EXPECT_EQ(CKR_OK, rv2);

    size_t decryptedLen = getRSAdecryptedLength();
    uint8_t *decrypted = nullptr;
    // RSA decryption: user1
    CK_RV rv3 = RSAdecrypt(user1, rsaKeyIds.second, encrypted, encryptedLen,
                           decrypted, &decryptedLen);
    EXPECT_EQ(CKR_EMPTY_BUFFER, rv3)
        << "Expected CKR_EMPTY_BUFFER for null data";

    delete[] encrypted;
    delete[] decrypted;
}

TEST_F(CryptoAPIFixture, rsaEncryptionWithInvalidKey)
{
    uint8_t data[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    size_t dataLen = 6;
    //printBufferHexa(data, dataLen, "rsa plain data");

    size_t encryptedLen = getRSAencryptedLength();
    uint8_t *encrypted = new uint8_t[encryptedLen];

    // RSA encryption: user2 -> user1
    CK_RV rvInvalidKey = RSAencrypt(user2, "InvalidKeyId", data, dataLen,
                                    encrypted, encryptedLen);

    EXPECT_EQ(CKR_USER_NOT_AUTHORIZED, rvInvalidKey)
        << "Expected CKR_USER_NOT_AUTHORIZED for invalid key ID";

    delete[] encrypted;
}

TEST_F(CryptoAPIFixture, rsaDecryptionWithInvalidKey)
{
    uint8_t data[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    size_t dataLen = 6;
    //printBufferHexa(data, dataLen, "rsa plain data");

    size_t encryptedLen = getRSAencryptedLength();
    uint8_t *encrypted = new uint8_t[encryptedLen];

    // RSA encryption: user2 -> user1
    CK_RV rv2 = RSAencrypt(user2, rsaKeyIds.first, data, dataLen, encrypted,
                           encryptedLen);
    EXPECT_EQ(CKR_OK, rv2);

    size_t decryptedLen = getRSAdecryptedLength();
    uint8_t *decrypted = new uint8_t[decryptedLen];

    CK_RV rvInvalidKey = RSAdecrypt(user1, "InvalidKeyId", encrypted,
                                    encryptedLen, decrypted, &decryptedLen);
    EXPECT_EQ(CKR_USER_NOT_AUTHORIZED, rvInvalidKey)
        << "Expected CKR_USER_NOT_AUTHORIZED for invalid key ID";

    delete[] encrypted;
    delete[] decrypted;
}

#endif  // RSA_TEST

#pragma endregion RSA tests

#pragma region ECC tests

#ifdef ECC_TEST
TEST_F(CryptoAPIFixture, eccEncryptionDecryption)
{
    // Generate ECC key pair for user1

    size_t dataLen = 16;
    uint8_t data[dataLen];
    memset(data, 1, dataLen);

    //printBufferHexa(data, dataLen, "ecc plain data");

    size_t encryptedLen = getECCencryptedLength();
    uint8_t *encrypted = new uint8_t[encryptedLen];

    // ECC encryption: user2 -> user1
    CK_RV rv2 = ECCencrypt(user2, eccKeyIds.first, data, dataLen, encrypted,
                           encryptedLen);
    //printBufferHexa(encrypted, encryptedLen, "ecc encrypted");
    EXPECT_EQ(CKR_OK, rv2);

    size_t decryptedLen = getECCdecryptedLength();
    uint8_t *decrypted = new uint8_t[decryptedLen];

    // ECC decryption: user1
    CK_RV rv3 = ECCdecrypt(user1, eccKeyIds.second, encrypted, encryptedLen,
                           decrypted, decryptedLen);
    EXPECT_EQ(CKR_OK, rv3);

    //printBufferHexa(decrypted, decryptedLen, "ecc decrypted");

    EXPECT_EQ(0, memcmp(data, decrypted, dataLen));

    delete[] encrypted;
    delete[] decrypted;
}

TEST_F(CryptoAPIFixture, EccEncryptionWithEmptyData)
{
    // Generate ECC key pair for user1

    size_t dataLen = 0;
    uint8_t *emptyData = nullptr;

    size_t encryptedLen = getECCencryptedLength();
    uint8_t *encrypted = new uint8_t[encryptedLen];

    // ECC encryption: user2 -> user1
    CK_RV rv = ECCencrypt(user2, eccKeyIds.first, emptyData, dataLen, encrypted,
                          encryptedLen);
    EXPECT_EQ(CKR_EMPTY_BUFFER, rv)
        << "Expected CKR_EMPTY_BUFFER for empty data";
}

TEST_F(CryptoAPIFixture, EccDecryptionWithEmptyData)
{
    // Generate ECC key pair for user1

    size_t dataLen = 16;
    uint8_t data[dataLen];
    memset(data, 1, dataLen);

    printBufferHexa(data, dataLen, "ecc plain data");

    size_t encryptedLen = getECCencryptedLength();
    uint8_t *encrypted = new uint8_t[encryptedLen];

    // ECC encryption: user2 -> user1
    CK_RV rv2 = ECCencrypt(user2, eccKeyIds.first, data, dataLen, encrypted,
                           encryptedLen);
    printBufferHexa(encrypted, encryptedLen, "ecc encrypted");
    EXPECT_EQ(CKR_OK, rv2);

    size_t decryptedLen = 0;
    uint8_t *decrypted = nullptr;

    // ECC decryption: user1
    CK_RV rv3 = ECCdecrypt(user1, eccKeyIds.second, encrypted, encryptedLen,
                           decrypted, decryptedLen);
    EXPECT_EQ(CKR_EMPTY_BUFFER, rv3)
        << "Expected CKR_EMPTY_BUFFER for empty data";

    delete[] encrypted;
}

TEST_F(CryptoAPIFixture, EccEncryptionWithInvalidKey)
{
    // Generate ECC key pair for user1

    size_t dataLen = 16;
    uint8_t data[dataLen];
    memset(data, 1, dataLen);

    printBufferHexa(data, dataLen, "ecc plain data");

    size_t encryptedLen = getECCencryptedLength();
    uint8_t *encrypted = new uint8_t[encryptedLen];

    // ECC encryption: user2 -> user1
    CK_RV rvInvalidKey = ECCencrypt(user2, "invalidKeyId", data, dataLen,
                                    encrypted, encryptedLen);
    EXPECT_EQ(CKR_USER_NOT_AUTHORIZED, rvInvalidKey)
        << "Expected CKR_USER_NOT_AUTHORIZED for invalid key ID";

    delete[] encrypted;
}

TEST_F(CryptoAPIFixture, EccDecryptionWithInvalidKey)
{
    // Generate ECC key pair for user1

    size_t dataLen = 16;
    uint8_t data[dataLen];
    memset(data, 1, dataLen);

    printBufferHexa(data, dataLen, "ecc plain data");

    size_t encryptedLen = getECCencryptedLength();
    uint8_t *encrypted = new uint8_t[encryptedLen];

    // ECC encryption: user2 -> user1
    CK_RV rv2 = ECCencrypt(user2, eccKeyIds.first, data, dataLen, encrypted,
                           encryptedLen);
    printBufferHexa(encrypted, encryptedLen, "ecc encrypted");
    EXPECT_EQ(CKR_OK, rv2);

    size_t decryptedLen = getECCdecryptedLength();
    uint8_t *decrypted = new uint8_t[decryptedLen];

    // ECC decryption: user1
    CK_RV rvInvalidKey = ECCdecrypt(user1, "invalidKeyId", encrypted,
                                    encryptedLen, decrypted, decryptedLen);
    EXPECT_EQ(CKR_USER_NOT_AUTHORIZED, rvInvalidKey)
        << "Expected CKR_USER_NOT_AUTHORIZED for invalid key ID";

    delete[] encrypted;
    delete[] decrypted;
}

#endif  // ECC_TEST

#pragma endregion ECC tests

#pragma region sign verify tests

#ifdef SIGN_VERIFY_TEST
TEST_F(CryptoAPIFixture, SignVerifySingleChunkTest)
{
    CK_RV rv;
    // Define the data size to be within a single chunk (e.g., 32KB)
    size_t singleChunkDataLen =
        32 * 1024;  // 32KB, smaller than the 64KB chunk size
    std::vector<uint8_t> singleChunkData(singleChunkDataLen,
                                         0xAB);  // Fill the data with 0xAB

    // Define chunk size (same as the data size, ensuring only one chunk)
    size_t chunkSize =
        singleChunkDataLen;  // For single chunk, chunk size is equal to data size
    size_t numChunks = 1;    // Only one chunk in this case

    // Buffer for signature
    size_t signatureLen = getSignatureLength();
    uint8_t
        signature[signatureLen];  // Assuming RSA-2048 for a 256-byte signature

    // Sign the single chunk
    std::cout << "Signing a single chunk..." << std::endl;
    size_t offset = 0;  // No offset needed for a single chunk
    rv = signUpdate(user1, &singleChunkData[offset], singleChunkDataLen,
                    SHA_256, 0);  // Only one call to signUpdate
    EXPECT_EQ(CKR_OK, rv) << "signUpdate failed for the single chunk";

    // Finalize signing
    rv =
        signFinalize(user1, signature, signatureLen, SHA_256, rsaKeyIds.second);
    EXPECT_EQ(CKR_OK, rv) << "signFinalize failed";

    // Verify the single chunk
    std::cout << "Verifying a single chunk..." << std::endl;
    rv = verifyUpdate(user2, &singleChunkData[offset], singleChunkDataLen,
                      SHA_256, 0);  // Only one call to verifyUpdate
    EXPECT_EQ(CKR_OK, rv) << "verifyUpdate failed for the single chunk";

    // Finalize verifying
    rv = verifyFinalize(user2, signature, signatureLen, SHA_256,
                        rsaKeyIds.first);
    EXPECT_EQ(CKR_OK, rv) << "verifyFinalize failed";

    // If all assertions pass, signing and verification of the single chunk were successful
    std::cout << "Sign and Verify for a single chunk Passed" << std::endl;
}

TEST_F(CryptoAPIFixture, SignVerifySingleChunkTinyDataTest)
{
    CK_RV rv;
    // Define a very small data size (16 bytes)
    size_t tinyDataLen = 16;  // 16 bytes
    std::vector<uint8_t> tinyData(tinyDataLen, 0xAB);  // Fill the data with 0xAB

    // Define chunk size (same as the data size, ensuring only one chunk)
    size_t chunkSize = tinyDataLen;  // For single chunk, chunk size is equal to data size
    size_t numChunks = 1;    // Only one chunk in this case

    // Buffer for signature
    size_t signatureLen = getSignatureLength();
    uint8_t signature[signatureLen];  // Assuming RSA-2048 for a 256-byte signature

    // Sign the single tiny chunk
    std::cout << "Signing a single tiny chunk (16 bytes)..." << std::endl;
    size_t offset = 0;  // No offset needed for a single chunk
    rv = signUpdate(user1, &tinyData[offset], tinyDataLen, SHA_256, 0);  // Only one call to signUpdate
    EXPECT_EQ(CKR_OK, rv) << "signUpdate failed for the tiny chunk";

    // Finalize signing
    rv = signFinalize(user1, signature, signatureLen, SHA_256, rsaKeyIds.second);
    EXPECT_EQ(CKR_OK, rv) << "signFinalize failed";

    // Verify the single tiny chunk
    std::cout << "Verifying a single tiny chunk (16 bytes)..." << std::endl;
    rv = verifyUpdate(user2, &tinyData[offset], tinyDataLen, SHA_256, 0);  // Only one call to verifyUpdate
    EXPECT_EQ(CKR_OK, rv) << "verifyUpdate failed for the tiny chunk";

    // Finalize verifying
    rv = verifyFinalize(user2, signature, signatureLen, SHA_256, rsaKeyIds.first);
    EXPECT_EQ(CKR_OK, rv) << "verifyFinalize failed";

    // If all assertions pass, signing and verification of the tiny chunk were successful
    std::cout << "Sign and Verify for a single tiny chunk Passed" << std::endl;
}

TEST_F(CryptoAPIFixture, SignUpdateWithNullData)
{
    // Define the data size to be within a single chunk (e.g., 32KB)
    size_t singleChunkDataLen = 0;
    uint8_t *nullData = nullptr;

    // Sign the single chunk
    std::cout << "Signing a single chunk..." << std::endl;
    printBufferHexa(nullData, singleChunkDataLen, "SHA");
    CK_RV rv = signUpdate(user1, nullData, singleChunkDataLen, SHA_256, 0);
    EXPECT_EQ(CKR_EMPTY_BUFFER, rv)
        << "Expected CKR_EMPTY_BUFFER for null data during signing";
}

TEST_F(CryptoAPIFixture, SignFinalizeWithNullData)
{
    CK_RV rv;
    // Define the data size to be within a single chunk (e.g., 32KB)
    size_t singleChunkDataLen =
        32 * 1024;  // 32KB, smaller than the 64KB chunk size
    std::vector<uint8_t> singleChunkData(singleChunkDataLen,
                                         0xAB);  // Fill the data with 0xAB

    // Define chunk size (same as the data size, ensuring only one chunk)
    size_t chunkSize =
        singleChunkDataLen;  // For single chunk, chunk size is equal to data size
    size_t numChunks = 1;    // Only one chunk in this case

    // Buffer for signature
    size_t signatureLen = getSignatureLength();
    uint8_t
        signature[signatureLen];  // Assuming RSA-2048 for a 256-byte signature

    // Sign the single chunk
    std::cout << "Signing a single chunk..." << std::endl;
    size_t offset = 0;  // No offset needed for a single chunk
    rv = signUpdate(user1, &singleChunkData[offset], singleChunkDataLen,
                    SHA_256, 0);  // Only one call to signUpdate
    EXPECT_EQ(CKR_OK, rv) << "signUpdate failed for the single chunk";

    size_t dataLen = 0;
    uint8_t *nullData = nullptr;

    // Finalize signing
    rv = signFinalize(user1, nullData, dataLen, SHA_256, rsaKeyIds.second);
    EXPECT_EQ(CKR_EMPTY_BUFFER, rv)
        << "Expected CKR_EMPTY_BUFFER for null data during signing";
}

TEST_F(CryptoAPIFixture, VerifyUpdateWithNullData)
{
    CK_RV rv;
    // Define the data size to be within a single chunk (e.g., 32KB)
    size_t singleChunkDataLen =
        32 * 1024;  // 32KB, smaller than the 64KB chunk size
    std::vector<uint8_t> singleChunkData(singleChunkDataLen,
                                         0xAB);  // Fill the data with 0xAB

    // Define chunk size (same as the data size, ensuring only one chunk)
    size_t chunkSize =
        singleChunkDataLen;  // For single chunk, chunk size is equal to data size
    size_t numChunks = 1;    // Only one chunk in this case

    // Buffer for signature
    size_t signatureLen = getSignatureLength();
    uint8_t signature[signatureLen];

    // Sign the single chunk
    std::cout << "Signing a single chunk..." << std::endl;
    size_t offset = 0;  // No offset needed for a single chunk
    rv = signUpdate(user1, &singleChunkData[offset], singleChunkDataLen,
                    SHA_256, 0);  // Only one call to signUpdate
    EXPECT_EQ(CKR_OK, rv) << "signUpdate failed for the single chunk";

    // Finalize signing
    rv =
        signFinalize(user1, signature, signatureLen, SHA_256, rsaKeyIds.second);
    EXPECT_EQ(CKR_OK, rv) << "signFinalize failed";

    uint8_t *nullData = nullptr;
    size_t dataSize = 0;
    // Verify the single chunk
    std::cout << "Verifying a single chunk..." << std::endl;
    rv = verifyUpdate(user2, nullData, dataSize, SHA_256, 0);
    EXPECT_EQ(CKR_EMPTY_BUFFER, rv)
        << "Expected CKR_EMPTY_BUFFER for null data during verifying";
}

TEST_F(CryptoAPIFixture, VerifyFinalizeWithNullData)
{
    CK_RV rv;
    // Define the data size to be within a single chunk (e.g., 32KB)
    size_t singleChunkDataLen =
        32 * 1024;  // 32KB, smaller than the 64KB chunk size
    std::vector<uint8_t> singleChunkData(singleChunkDataLen,
                                         0xAB);  // Fill the data with 0xAB

    // Define chunk size (same as the data size, ensuring only one chunk)
    size_t chunkSize =
        singleChunkDataLen;  // For single chunk, chunk size is equal to data size
    size_t numChunks = 1;    // Only one chunk in this case

    // Buffer for signature
    size_t signatureLen = getSignatureLength();
    uint8_t
        signature[signatureLen];  // Assuming RSA-2048 for a 256-byte signature

    // Sign the single chunk
    std::cout << "Signing a single chunk..." << std::endl;
    size_t offset = 0;  // No offset needed for a single chunk
    rv = signUpdate(user1, &singleChunkData[offset], singleChunkDataLen,
                    SHA_256, 0);  // Only one call to signUpdate
    EXPECT_EQ(CKR_OK, rv) << "signUpdate failed for the single chunk";

    // Finalize signing
    rv =
        signFinalize(user1, signature, signatureLen, SHA_256, rsaKeyIds.second);
    EXPECT_EQ(CKR_OK, rv) << "signFinalize failed";

    // Verify the single chunk
    std::cout << "Verifying a single chunk..." << std::endl;
    rv = verifyUpdate(user2, &singleChunkData[offset], singleChunkDataLen,
                      SHA_256, 0);  // Only one call to verifyUpdate
    EXPECT_EQ(CKR_OK, rv) << "verifyUpdate failed for the single chunk";

    size_t dataLen = 0;
    uint8_t *nullData = nullptr;

    // Finalize verifying
    rv = verifyFinalize(user2, nullData, dataLen, SHA_256, rsaKeyIds.first);
    EXPECT_EQ(CKR_EMPTY_BUFFER, rv)
        << "Expected CKR_EMPTY_BUFFER for null data during signing";
}

TEST_F(CryptoAPIFixture, SignVerifyWithUnauthorizedUser)
{
    CK_RV rv;
    // Define the data size to be within a single chunk (e.g., 32KB)
    size_t singleChunkDataLen =
        32 * 1024;  // 32KB, smaller than the 64KB chunk size
    std::vector<uint8_t> singleChunkData(singleChunkDataLen,
                                         0xAB);  // Fill the data with 0xAB

    // Define chunk size (same as the data size, ensuring only one chunk)
    size_t chunkSize =
        singleChunkDataLen;  // For single chunk, chunk size is equal to data size
    size_t numChunks = 1;    // Only one chunk in this case

    // Buffer for signature
    size_t signatureLen = getSignatureLength();
    uint8_t
        signature[signatureLen];  // Assuming RSA-2048 for a 256-byte signature

    // Sign the single chunk
    std::cout << "Signing a single chunk..." << std::endl;
    size_t offset = 0;  // No offset needed for a single chunk
    rv = signUpdate(4, &singleChunkData[offset], singleChunkDataLen, SHA_256,
                    0);  // Only one call to signUpdate
    EXPECT_EQ(CKR_OK, rv) << "signUpdate failed for the single chunk";

    // Finalize signing
    rv = signFinalize(4, signature, signatureLen, SHA_256, rsaKeyIds.second);
    EXPECT_EQ(CKR_USER_NOT_AUTHORIZED, rv)
        << "Expected CKR_USER_NOT_AUTHORIZED for unauthorized verifying";
}

// Test for sign and verify functions
TEST_F(CryptoAPIFixture, SignVerifyChunkedTest)
{
    CK_RV rv;
    // Simulate large data (e.g., 1MB)
    size_t largeDataLen = 1024 * 1024;  // 1MB
    std::vector<uint8_t> largeData(
        largeDataLen, 0xAB);  // Fill the data with 0xAB for simulation

    // Define chunk size (e.g., 64KB)
    size_t chunkSize = 64 * 1024;
    size_t numChunks = (largeDataLen + chunkSize - 1) /
                       chunkSize;  // Calculate the number of chunks

    // Buffer for signature
    size_t signatureLen = getSignatureLength();
    uint8_t
        signature[signatureLen];  // Assuming RSA-2048 for a 256-byte signature

    // Sign in chunks
    std::cout << "Signing in chunks..." << std::endl;
    for (size_t i = 0; i < numChunks; ++i) {
        size_t offset = i * chunkSize;
        size_t currentChunkSize = std::min(
            chunkSize, largeDataLen - offset);  // Handle last chunk size
        rv =
            signUpdate(user1, &largeData[offset], currentChunkSize, SHA_256, i);
        EXPECT_EQ(CKR_OK, rv) << "signUpdate failed for chunk " << i;
    }

    // Finalize signing, user1
    rv =
        signFinalize(user1, signature, signatureLen, SHA_256, rsaKeyIds.second);
    EXPECT_EQ(CKR_OK, rv) << "signFinalize failed";

    // Now, let's verify in chunks
    std::cout << "Verifying in chunks..." << std::endl;
    for (size_t i = 0; i < numChunks; ++i) {
        size_t offset = i * chunkSize;
        size_t currentChunkSize = std::min(chunkSize, largeDataLen - offset);
        rv = verifyUpdate(user2, &largeData[offset], currentChunkSize, SHA_256,
                          i);
        EXPECT_EQ(CKR_OK, rv) << "verifyUpdate failed for chunk " << i;
    }

    // Finalize verifying
    rv = verifyFinalize(user2, signature, signatureLen, SHA_256,
                        rsaKeyIds.first);
    EXPECT_EQ(CKR_OK, rv) << "verifyFinalize failed";

    // If all assertions pass, signing and verification of large data in chunks
    // were successful
    std::cout << "Sign and Verify for large data in chunks Passed" << std::endl;
}
#endif  // SIGN_VERIFY_TEST

#pragma endregion sign verify tests

#pragma region AES tests

void testEncryptionDecryptionAPI(AESChainingMode mode, AESKeyLength keyLength)
{
    int user1 = 1;
    int user2 = 2;
    size_t counter = 1;
    std::vector<KeyPermission> permissions = {
        KeyPermission::DECRYPT, KeyPermission::ENCRYPT, KeyPermission::SIGN,
        KeyPermission::VERIFY, KeyPermission::EXPORTABLE};
    AESChainingMode chainingMode = mode;  // Change as needed
    std::string keyId = generateAESKey(user1, keyLength, permissions, user2);
    size_t inputLength1 = 64;
    unsigned char inputData1[inputLength1];
    memset(inputData1, 0x02, inputLength1);
    size_t inputLength2 = 32;
    unsigned char inputData2[inputLength2];
    memset(inputData2, 0x02, inputLength2);
    size_t inputLength3 = 32;
    unsigned char inputData3[inputLength3];
    memset(inputData3, 0x02, inputLength3);

    size_t encryptedLength1 = getAESencryptedLength(inputLength1, true, mode);
    uint8_t encryptedData1[encryptedLength1];
    size_t encryptedLength2 = getAESencryptedLength(inputLength2, false, mode);
    uint8_t encryptedData2[encryptedLength2];
    size_t encryptedLength3 = getAESencryptedLength(inputLength3, false, mode);
    uint8_t encryptedData3[encryptedLength3];
    counter = 3;

    // Encrypt the data
    CK_RV result1 = AESencrypt(user1, user2, (void *)inputData1, inputLength1,
                               encryptedData1, encryptedLength1, keyLength,
                               chainingMode, counter, keyId);

    // Check for successful encryption
    EXPECT_EQ(result1, CKR_OK);
    // Encrypt the data
    CK_RV result2 = AESencrypt(user1, user2, (void *)inputData2, inputLength2,
                               encryptedData2, encryptedLength2, keyLength,
                               chainingMode, counter, keyId);

    // Check for successful encryption
    EXPECT_EQ(result2, CKR_OK);  // Encrypt the data
    CK_RV result3 = AESencrypt(user1, user2, (void *)inputData3, inputLength3,
                               encryptedData3, encryptedLength3, keyLength,
                               chainingMode, counter, keyId);

    // Check for successful encryption
    EXPECT_EQ(result3, CKR_OK);

    // Decrypt the data
    size_t decryptedLength1 =
        getAESdecryptedLength(encryptedLength1, true, mode);
    size_t decryptedLength2 =
        getAESdecryptedLength(encryptedLength2, false, mode);
    size_t decryptedLength3 =
        getAESdecryptedLength(encryptedLength3, false, mode);
    //printBufferHexa(encryptedData1, encryptedLength1,
    //                "Encrypted data1 aes through api");
    //printBufferHexa(encryptedData2, encryptedLength2,
    //                "Encrypted data2 aes through api");
    //printBufferHexa(encryptedData3, encryptedLength3,
    //                "Encrypted data3 aes through api");
    uint8_t decryptedData1[decryptedLength1];
    uint8_t decryptedData2[decryptedLength2];
    uint8_t decryptedData3[decryptedLength3];

    result1 = AESdecrypt(user1, user2, encryptedData1, encryptedLength1,
                         decryptedData1, decryptedLength1, keyLength,
                         chainingMode, counter, keyId);
    EXPECT_EQ(result1, CKR_OK);
    // printBufferHexa(inputData1, inputLength1, "Original Data1: ");
    // printBufferHexa(decryptedData1, decryptedLength1, "Decrypted Data1: ");
    result2 = AESdecrypt(user1, user2, encryptedData2, encryptedLength2,
                         decryptedData2, decryptedLength2, keyLength,
                         chainingMode, counter, keyId);
    EXPECT_EQ(result2, CKR_OK);

    // printBufferHexa(inputData2, inputLength2, "Original Data2: ");
    // printBufferHexa(decryptedData2, decryptedLength2, "Decrypted Data2: ");
    result3 = AESdecrypt(user1, user2, encryptedData3, encryptedLength3,
                         decryptedData3, decryptedLength3, keyLength,
                         chainingMode, counter, keyId);
    // printBufferHexa(inputData3, inputLength3, "Original Data3: ");
    // printBufferHexa(decryptedData3, decryptedLength3, "Decrypted Data3: ");

    // Check for successful decryption
    EXPECT_EQ(result3, CKR_OK);

    // Verify the decrypted data matches the original input
    EXPECT_EQ(memcmp(inputData1, decryptedData1, decryptedLength1), 0);
    EXPECT_EQ(memcmp(inputData2, decryptedData2, decryptedLength2), 0);
    EXPECT_EQ(memcmp(inputData3, decryptedData3, decryptedLength3), 0);
    EXPECT_EQ(inputLength1, decryptedLength1);
    EXPECT_EQ(inputLength2, decryptedLength2);
    EXPECT_EQ(inputLength3, decryptedLength3);
};

#define AES_TESTS

#ifdef AES_TESTS

TEST(KeyLengthsAPI, KeyLength128_ECB)
{
    testEncryptionDecryptionAPI(AESChainingMode::ECB, AESKeyLength::AES_128);
}

TEST(KeyLengthsAPI, KeyLength128_CBC)
{
    testEncryptionDecryptionAPI(AESChainingMode::CBC, AESKeyLength::AES_128);
}

TEST(KeyLengthsAPI, KeyLength128_CFB)
{
    testEncryptionDecryptionAPI(AESChainingMode::CFB, AESKeyLength::AES_128);
}

TEST(KeyLengthsAPI, KeyLength128_OFB)
{
    testEncryptionDecryptionAPI(AESChainingMode::OFB, AESKeyLength::AES_128);
}

TEST(KeyLengthsAPI, KeyLength128_CTR)
{
    testEncryptionDecryptionAPI(AESChainingMode::CTR, AESKeyLength::AES_128);
}

TEST(KeyLengthsAPI, KeyLength192_ECB)
{
    testEncryptionDecryptionAPI(AESChainingMode::ECB, AESKeyLength::AES_192);
}

TEST(KeyLengthsAPI, KeyLength192_CBC)
{
    testEncryptionDecryptionAPI(AESChainingMode::CBC, AESKeyLength::AES_192);
}

TEST(KeyLengthsAPI, KeyLength192_CFB)
{
    testEncryptionDecryptionAPI(AESChainingMode::CFB, AESKeyLength::AES_192);
}

TEST(KeyLengthsAPI, KeyLength192_OFB)
{
    testEncryptionDecryptionAPI(AESChainingMode::OFB, AESKeyLength::AES_192);
}

TEST(KeyLengthsAPI, KeyLength192_CTR)
{
    testEncryptionDecryptionAPI(AESChainingMode::CTR, AESKeyLength::AES_192);
}

TEST(KeyLengthsAPI, KeyLength256_ECB)
{
    testEncryptionDecryptionAPI(AESChainingMode::ECB, AESKeyLength::AES_256);
}

TEST(KeyLengthsAPI, KeyLength256_CBC)
{
    testEncryptionDecryptionAPI(AESChainingMode::CBC, AESKeyLength::AES_256);
}

TEST(KeyLengthsAPI, KeyLength256_CFB)
{
    testEncryptionDecryptionAPI(AESChainingMode::CFB, AESKeyLength::AES_256);
}

TEST(KeyLengthsAPI, KeyLength256_OFB)
{
    testEncryptionDecryptionAPI(AESChainingMode::OFB, AESKeyLength::AES_256);
}

TEST(KeyLengthsAPI, KeyLength256_CTR)
{
    testEncryptionDecryptionAPI(AESChainingMode::CTR, AESKeyLength::AES_256);
}

TEST_F(CryptoAPIFixture, aesEncryptionWithNullInputData)
{
    int user1 = 1;
    int user2 = 2;
    size_t counter = 1;
    std::vector<KeyPermission> permissions = {
        KeyPermission::DECRYPT, KeyPermission::ENCRYPT};
    AESKeyLength keyLength = AESKeyLength::AES_256;
    AESChainingMode chainingMode = AESChainingMode::CBC;
    std::string keyId = generateAESKey(user1, keyLength, permissions, user2);

    unsigned char* nullData = nullptr;
    size_t inputLength = 0;

    size_t encryptedLength = getAESencryptedLength(inputLength, true, chainingMode);
    uint8_t* encryptedData = new uint8_t[encryptedLength];

    CK_RV result = AESencrypt(user1, user2, (void*)nullData, inputLength, 
                              encryptedData, encryptedLength, keyLength, 
                              chainingMode, counter, keyId);

    EXPECT_EQ(result, CKR_EMPTY_BUFFER);

    delete[] encryptedData;
}

TEST_F(CryptoAPIFixture, aesDecryptionWithNullInputData)
{
    int user1 = 1;
    int user2 = 2;
    size_t counter = 1;
    std::vector<KeyPermission> permissions = {
        KeyPermission::DECRYPT, KeyPermission::ENCRYPT};
    AESKeyLength keyLength = AESKeyLength::AES_256;
    AESChainingMode chainingMode = AESChainingMode::CBC;
    std::string keyId = generateAESKey(user1, keyLength, permissions, user2);

    unsigned char* nullEncryptedData = nullptr;
    size_t encryptedLength = 80;

    size_t decryptedLength = getAESdecryptedLength(encryptedLength, true, chainingMode);
    uint8_t* decryptedData = new uint8_t[decryptedLength];

    CK_RV result = AESdecrypt(user1, user2, (void*)nullEncryptedData, encryptedLength, 
                              decryptedData, decryptedLength, keyLength, 
                              chainingMode, counter, keyId);

    EXPECT_EQ(result, CKR_EMPTY_BUFFER);

    delete[] decryptedData;
}


TEST_F(CryptoAPIFixture, aesEncryptionWithInvalidKeyLength)
{
    int user1 = 1;
    int user2 = 2;
    size_t counter = 1;
    std::vector<KeyPermission> permissions = {
        KeyPermission::DECRYPT, KeyPermission::ENCRYPT};
    AESKeyLength invalidKeyLength = static_cast<AESKeyLength>(999); // Invalid key length
    AESChainingMode chainingMode = AESChainingMode::CBC;
    std::string keyId = generateAESKey(user1, AESKeyLength::AES_256, permissions, user2);

    unsigned char inputData[] = {0x01, 0x02, 0x03, 0x04};
    size_t inputLength = sizeof(inputData);

    size_t encryptedLength = getAESencryptedLength(inputLength, true, chainingMode);
    uint8_t* encryptedData = new uint8_t[encryptedLength];

    CK_RV result = AESencrypt(user1, user2, (void*)inputData, inputLength, 
                              encryptedData, encryptedLength, invalidKeyLength, 
                              chainingMode, counter, keyId);

    EXPECT_EQ(result, CKR_KEY_SIZE_RANGE); // Assuming error for invalid key length

    delete[] encryptedData;
}

TEST_F(CryptoAPIFixture, aesDecryptionWithModifiedEncryptedData)
{
    int user1 = 1;
    int user2 = 2;
    size_t counter = 1;
    std::vector<KeyPermission> permissions = {
        KeyPermission::DECRYPT, KeyPermission::ENCRYPT};
    AESKeyLength keyLength = AESKeyLength::AES_256;
    AESChainingMode chainingMode = AESChainingMode::CBC;
    std::string keyId = generateAESKey(user1, keyLength, permissions, user2);

    unsigned char inputData[] = {0x01, 0x02, 0x03, 0x04};
    size_t inputLength = sizeof(inputData);

    size_t encryptedLength = getAESencryptedLength(inputLength, true, chainingMode);
    uint8_t* encryptedData = new uint8_t[encryptedLength];

    // Encrypt the data
    CK_RV result = AESencrypt(user1, user2, (void*)inputData, inputLength, 
                              encryptedData, encryptedLength, keyLength, 
                              chainingMode, counter, keyId);
    EXPECT_EQ(result, CKR_OK);

    // Modify encrypted data (simulating corruption)
    encryptedData[0] ^= 0xFF;

    size_t decryptedLength = getAESdecryptedLength(encryptedLength, true, chainingMode);
    uint8_t* decryptedData = new uint8_t[decryptedLength];

    // Attempt to decrypt corrupted data
    result = AESdecrypt(user1, user2, encryptedData, encryptedLength, 
                        decryptedData, decryptedLength, keyLength, 
                        chainingMode, counter, keyId);

    EXPECT_EQ(result, CKR_OK); //success    

    delete[] encryptedData;
    delete[] decryptedData;
}

TEST_F(CryptoAPIFixture, aesEncryptionWithZeroLengthInput)
{
    int user1 = 1;
    int user2 = 2;
    size_t counter = 1;
    std::vector<KeyPermission> permissions = {
        KeyPermission::DECRYPT, KeyPermission::ENCRYPT};
    AESKeyLength keyLength = AESKeyLength::AES_256;
    AESChainingMode chainingMode = AESChainingMode::CBC;
    std::string keyId = generateAESKey(user1, keyLength, permissions, user2);

    unsigned char inputData[] = {};  // Zero-length data
    size_t inputLength = 0;

    size_t encryptedLength = getAESencryptedLength(inputLength, true, chainingMode);
    uint8_t* encryptedData = new uint8_t[encryptedLength];

    CK_RV result = AESencrypt(user1, user2, (void*)inputData, inputLength, 
                              encryptedData, encryptedLength, keyLength, 
                              chainingMode, counter, keyId);

    EXPECT_EQ(result, CKR_EMPTY_BUFFER);

    delete[] encryptedData;
}

TEST_F(CryptoAPIFixture, aesDecryptionWithZeroLengthEncryptedData)
{
    int user1 = 1;
    int user2 = 2;
    size_t counter = 1;
    std::vector<KeyPermission> permissions = {
        KeyPermission::DECRYPT, KeyPermission::ENCRYPT};
    AESKeyLength keyLength = AESKeyLength::AES_256;
    AESChainingMode chainingMode = AESChainingMode::CBC;
    std::string keyId = generateAESKey(user1, keyLength, permissions, user2);

    uint8_t * encryptedData=nullptr;  // Zero-length data
    size_t encryptedLength = 0;

    size_t decryptedLength = getAESdecryptedLength(encryptedLength, true, chainingMode);
    uint8_t* decryptedData = new uint8_t[decryptedLength];

    CK_RV result = AESdecrypt(user1, user2, encryptedData, encryptedLength, 
                              decryptedData, decryptedLength, keyLength, 
                              chainingMode, counter, keyId);

    EXPECT_EQ(result, CKR_EMPTY_BUFFER);

    delete[] decryptedData;
}

#endif

#pragma endregion AES tests

#pragma region Encrypt decrypt tests

void GenericEncryptionDecryptionTest(CryptoConfig config)
{
    try {
        int user1 = 1;
        int user2 = 2;

        std::vector<KeyPermission> permissions = {
            KeyPermission::DECRYPT, KeyPermission::ENCRYPT, KeyPermission::SIGN,
            KeyPermission::VERIFY, KeyPermission::EXPORTABLE};

        configure(user1, config);  //give encrypt-decrypt behavior
        configure(user2, config);  //give encrypt-decrypt behavior
        bootSystem({{user1, permissions},
                    {user2, permissions}});  //generate keys with permissions

        size_t inputLength1 = 32;
        unsigned char inputData1[inputLength1];
        memset(inputData1, 0x01, inputLength1);
        size_t inputLength2 = 32;
        unsigned char inputData2[inputLength2];
        memset(inputData2, 0x02, inputLength2);
        size_t inputLength3 = 24;
        unsigned char inputData3[inputLength3];
        memset(inputData3, 0x03, inputLength3);

        size_t encryptedLength1 = getEncryptedLen(user1, inputLength1, true);
        if (encryptedLength1 == CKR_FUNCTION_FAILED)
            std::cout << "kfmjdfnvhfn";
        uint8_t encryptedData1[encryptedLength1];
        size_t encryptedLength2 = getEncryptedLen(user1, inputLength2, false);
        uint8_t encryptedData2[encryptedLength2];
        size_t encryptedLength3 = getEncryptedLen(user1, inputLength3, false);
        uint8_t encryptedData3[encryptedLength3];
        size_t counter = 3;

        size_t signatureLen = getSignatureLength();
        uint8_t *signature = new uint8_t[signatureLen];
        // Encrypt the data
        CK_RV result1 = encrypt(user1, user2, (void *)inputData1, inputLength1,
                                encryptedData1, encryptedLength1, signature,
                                signatureLen, counter);
        // printBufferHexa(encryptedData1, encryptedLength1,
        //                 "Encrypted data1 aes through api");
        // Check for successful encryption
        EXPECT_EQ(result1, CKR_OK);
        // Encrypt the data
        CK_RV result2 = encrypt(user1, user2, (void *)inputData2, inputLength2,
                                encryptedData2, encryptedLength2, signature,
                                signatureLen, counter);
        // printBufferHexa(encryptedData2, encryptedLength2,
        //                 "Encrypted data2 aes through api");
        // Check for successful encryption
        EXPECT_EQ(result2, CKR_OK);  // Encrypt the data
        CK_RV result3 = encrypt(user1, user2, (void *)inputData3, inputLength3,
                                encryptedData3, encryptedLength3, signature,
                                signatureLen, counter);
        // Check for successful encryption
        EXPECT_EQ(result3, CKR_OK);

        // printBufferHexa(encryptedData3, encryptedLength3,
        //                 "Encrypted data3 aes through api");
        // Decrypt the data
        size_t decryptedLength1 =
            getDecryptedLen(user1, encryptedLength1, true);
        size_t decryptedLength2 =
            getDecryptedLen(user1, encryptedLength2, false);
        size_t decryptedLength3 =
            getDecryptedLen(user1, encryptedLength3, false);

        uint8_t decryptedData1[decryptedLength1];
        uint8_t decryptedData2[decryptedLength2];
        uint8_t decryptedData3[decryptedLength3];

        result1 =
            decrypt(user1, user2, encryptedData1, encryptedLength1, signature,
                    signatureLen, decryptedData1, decryptedLength1, counter);
        EXPECT_EQ(result1, CKR_OK);
        // printBufferHexa(inputData1, inputLength1, "Original Data1: ");
        // printBufferHexa(decryptedData1, decryptedLength1, "Decrypted Data1: ");
        result2 =
            decrypt(user1, user2, encryptedData2, encryptedLength2, signature,
                    signatureLen, decryptedData2, decryptedLength2, counter);
        EXPECT_EQ(result2, CKR_OK);
        // printBufferHexa(inputData2, inputLength2, "Original Data2: ");
        // printBufferHexa(decryptedData2, decryptedLength2, "Decrypted Data2: ");
        result3 =
            decrypt(user1, user2, encryptedData3, encryptedLength3, signature,
                    signatureLen, decryptedData3, decryptedLength3, counter);

        //printBufferHexa(inputData3, inputLength3, "Original Data3: ");
        //printBufferHexa(decryptedData3, decryptedLength3, "Decrypted Data3: ");

        // Check for successful decryption
        EXPECT_EQ(result3, CKR_OK);

        // Verify the decrypted data matches the original input
        EXPECT_EQ(memcmp(inputData1, decryptedData1, decryptedLength1), 0);
        EXPECT_EQ(memcmp(inputData2, decryptedData2, decryptedLength2), 0);
        EXPECT_EQ(memcmp(inputData3, decryptedData3, decryptedLength3), 0);
        EXPECT_EQ(inputLength1, decryptedLength1);
        EXPECT_EQ(inputLength2, decryptedLength2);
        EXPECT_EQ(inputLength3, decryptedLength3);
    }
    catch (std::exception &e) {
        std::cerr << "Error::::::::::: " << e.what() << std::endl;
    }
}

void GenericEncryptionDecryptionTestWithEdgeCases(CryptoConfig config) {
    try {
        int user1 = 1;
        int user2 = 2;
        
        std::vector<KeyPermission> permissions = {KeyPermission::DECRYPT, KeyPermission::ENCRYPT};

        configure(user1, config); // give encrypt-decrypt behavior
        configure(user2, config); // give encrypt-decrypt behavior
        
        bootSystem({{user1, permissions}, {user2, permissions}}); // generate keys with permissions
        
        // Test case 1: Empty data encryption
        size_t emptyInputLength = 0;
        unsigned char emptyInputData[1];
        size_t emptyEncryptedLength = getEncryptedLen(user1, emptyInputLength, true);
        uint8_t emptyEncryptedData[emptyEncryptedLength];
        CK_RV resultEmpty = encrypt(user1, user2, (void *)emptyInputData, emptyInputLength, emptyEncryptedData, emptyEncryptedLength, nullptr, 0, 0);
        EXPECT_EQ(resultEmpty, CKR_EMPTY_BUFFER); // Check for successful encryption with empty data
        
        // // Test case 2: Encryption with different counter values
        // size_t inputLengthCounter = 16;
        // unsigned char inputDataCounter[inputLengthCounter];
        // memset(inputDataCounter, 0x04, inputLengthCounter);
        // size_t encryptedLengthCounter = getEncryptedLen(user1, inputLengthCounter, true);
        // uint8_t encryptedDataCounter[encryptedLengthCounter];
        // size_t counterValues[] = {1, 10, 100};
        // CK_RV resultCounter = CKR_FUNCTION_FAILED;
        // for (size_t counter : counterValues) {
        //     resultCounter = encrypt(user1, user2, (void *)inputDataCounter, inputLengthCounter, encryptedDataCounter, encryptedLengthCounter, nullptr, 0, counter);
        //     EXPECT_EQ(resultCounter, CKR_FUNCTION_FAILED); // Check for successful encryption with different counter values
        // }
        
        // Test case 3: Encryption with maximum data length
        size_t maxInputLength = 1024;
        unsigned char maxInputData[maxInputLength];
        memset(maxInputData, 0xFF, maxInputLength);
        size_t encryptedLengthMax = getEncryptedLen(user1, maxInputLength, true);
        uint8_t encryptedDataMax[encryptedLengthMax];
        CK_RV resultMax = encrypt(user1, user2, (void *)maxInputData, maxInputLength, encryptedDataMax, encryptedLengthMax, nullptr, 0, 0);
        EXPECT_EQ(resultMax, CKR_OK); // Check for successful encryption with maximum data length
        
        // Test case 4: Encryption and decryption with random data
        size_t randomInputLength = 64;
        unsigned char randomInputData[randomInputLength];
        srand(time(0));
        for (size_t i = 0; i < randomInputLength; ++i) {
            randomInputData[i] = rand() % 256; // Generate random data
        }
        size_t encryptedLengthRandom = getEncryptedLen(user1, randomInputLength, true);
        uint8_t encryptedDataRandom[encryptedLengthRandom];
        CK_RV resultRandomEncrypt = encrypt(user1, user2, (void *)randomInputData, randomInputLength, encryptedDataRandom, encryptedLengthRandom, nullptr, 0, 0);
        EXPECT_EQ(resultRandomEncrypt, CKR_OK); // Check for successful encryption with random data
        
        // Test case 5: Decryption with incorrect signature
        size_t decryptedLengthRandom = getDecryptedLen(user1, encryptedLengthRandom, true);
        uint8_t decryptedDataRandom[decryptedLengthRandom];
        uint8_t fakeSignature[16]; // Incorrect signature
        CK_RV resultRandomDecrypt = decrypt(user1, user2, encryptedDataRandom, encryptedLengthRandom, fakeSignature, sizeof(fakeSignature), decryptedDataRandom, decryptedLengthRandom, 0);
        EXPECT_NE(resultRandomDecrypt, CKR_OK); // Check for unsuccessful decryption with incorrect signature
        
        // Add more test cases for different edge cases as needed
        
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
// Control macros to enable or disable RSA and ECC tests

#define RUN_RSA_TESTS  // Set to 1 to run RSA tests, 0 to skip
#define RUN_ECC_TESTS    // Set to 1 to run ECC tests, 0 to skip

// TEST(EncryptDecryptAPIWithEdgeCases, SHA256_KeyLength128_ECB_RSA)
// {
//     CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_128,
//                         AESChainingMode::ECB, AsymmetricFunction::RSA);
//     GenericEncryptionDecryptionTestWithEdgeCases(config);
// }
//AES_128 combinations
#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength128_ECB_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_128,
                        AESChainingMode::ECB, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength128_ECB_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_128,
                        AESChainingMode::ECB, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength128_CBC_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_128,
                        AESChainingMode::CBC, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength128_CBC_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_128,
                        AESChainingMode::CBC, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength128_CFB_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_128,
                        AESChainingMode::CFB, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength128_CFB_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_128,
                        AESChainingMode::CFB, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength128_OFB_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_128,
                        AESChainingMode::OFB, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength128_OFB_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_128,
                        AESChainingMode::OFB, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength128_CTR_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_128,
                        AESChainingMode::CTR, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength128_CTR_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_128,
                        AESChainingMode::CTR, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

// AES_192 combinations
#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength192_ECB_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_192,
                        AESChainingMode::ECB, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength192_ECB_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_192,
                        AESChainingMode::ECB, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength192_CBC_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_192,
                        AESChainingMode::CBC, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength192_CBC_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_192,
                        AESChainingMode::CBC, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength192_CFB_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_192,
                        AESChainingMode::CFB, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength192_CFB_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_192,
                        AESChainingMode::CFB, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength192_OFB_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_192,
                        AESChainingMode::OFB, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength192_OFB_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_192,
                        AESChainingMode::OFB, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength192_CTR_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_192,
                        AESChainingMode::CTR, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength192_CTR_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_192,
                        AESChainingMode::CTR, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

// AES_256 combinations
#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength256_ECB_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_256,
                        AESChainingMode::ECB, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength256_ECB_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_256,
                        AESChainingMode::ECB, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength256_CBC_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_256,
                        AESChainingMode::CBC, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength256_CBC_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_256,
                        AESChainingMode::CBC, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength256_CFB_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_256,
                        AESChainingMode::CFB, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength256_CFB_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_256,
                        AESChainingMode::CFB, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength256_OFB_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_256,
                        AESChainingMode::OFB, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength256_OFB_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_256,
                        AESChainingMode::OFB, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength256_CTR_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_256,
                        AESChainingMode::CTR, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA256_KeyLength256_CTR_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_256,
                        AESChainingMode::CTR, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

// SHA_3_512 combinations for AES_128
#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength128_ECB_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_128,
                        AESChainingMode::ECB, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength128_ECB_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_128,
                        AESChainingMode::ECB, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength128_CBC_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_128,
                        AESChainingMode::CBC, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength128_CBC_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_128,
                        AESChainingMode::CBC, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength128_CFB_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_128,
                        AESChainingMode::CFB, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength128_CFB_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_128,
                        AESChainingMode::CFB, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength128_OFB_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_128,
                        AESChainingMode::OFB, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength128_OFB_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_128,
                        AESChainingMode::OFB, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength128_CTR_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_128,
                        AESChainingMode::CTR, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength128_CTR_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_128,
                        AESChainingMode::CTR, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

// SHA_3_512 combinations for AES_192
#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength192_ECB_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_192,
                        AESChainingMode::ECB, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength192_ECB_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_192,
                        AESChainingMode::ECB, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength192_CBC_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_192,
                        AESChainingMode::CBC, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength192_CBC_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_192,
                        AESChainingMode::CBC, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength192_CFB_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_192,
                        AESChainingMode::CFB, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength192_CFB_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_192,
                        AESChainingMode::CFB, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength192_OFB_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_192,
                        AESChainingMode::OFB, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength192_OFB_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_192,
                        AESChainingMode::OFB, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength192_CTR_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_192,
                        AESChainingMode::CTR, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength192_CTR_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_192,
                        AESChainingMode::CTR, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

// SHA_3_512 combinations for AES_256
#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength256_ECB_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_256,
                        AESChainingMode::ECB, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength256_ECB_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_256,
                        AESChainingMode::ECB, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength256_CBC_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_256,
                        AESChainingMode::CBC, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength256_CBC_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_256,
                        AESChainingMode::CBC, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength256_CFB_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_256,
                        AESChainingMode::CFB, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength256_CFB_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_256,
                        AESChainingMode::CFB, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength256_OFB_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_256,
                        AESChainingMode::OFB, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength256_OFB_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_256,
                        AESChainingMode::OFB, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_RSA_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength256_CTR_RSA)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_256,
                        AESChainingMode::CTR, AsymmetricFunction::RSA);
    GenericEncryptionDecryptionTest(config);
}
#endif

#ifdef RUN_ECC_TESTS
TEST(EncryptDecryptAPI, SHA3_512_KeyLength256_CTR_ECC)
{
    CryptoConfig config(SHAAlgorithm::SHA_3_512, AESKeyLength::AES_256,
                        AESChainingMode::CTR, AsymmetricFunction::ECC);
    GenericEncryptionDecryptionTest(config);
}
#endif

#pragma endregion Encrypt decrypt tests

TEST(BootSystem, emptyMap)
{
    bootSystem({});
}