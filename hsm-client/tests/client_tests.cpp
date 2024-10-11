#include "../include/crypto_api.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>

class CryptoClientTest : public ::testing::Test {
protected:
    CryptoClient client;
    int senderId = 1;
    int receiverId = 2;
    size_t messageLen = 2590;
    CryptoConfig config;

    CryptoClientTest()
        : client(),
          config(SHAAlgorithm::SHA_256, AESKeyLength::AES_128, AESChainingMode::ECB, AsymmetricFunction::RSA) {}

    void SetUp() override {
        client.configure(senderId, config);
        client.configure(receiverId, config);
        client.bootSystem({{senderId, {KeyPermission::VERIFY, KeyPermission::SIGN, KeyPermission::ENCRYPT, KeyPermission::DECRYPT, KeyPermission::EXPORTABLE}},
                           {receiverId, {KeyPermission::VERIFY, KeyPermission::SIGN, KeyPermission::ENCRYPT, KeyPermission::DECRYPT, KeyPermission::EXPORTABLE}}});
    }
};

// TEST_F(CryptoClientTest, EncryptDecrypt) 
// {
//     char* message = new char[messageLen]; 
//     std::memset(message, 'A', messageLen);
//     size_t encryptedLength = client.getEncryptedLenClient(senderId, messageLen);
//     uint8_t encryptedData[encryptedLength];
//     CK_RV encryptResult = client.encrypt(senderId, receiverId, (void *)message, messageLen, encryptedData, encryptedLength);
//     //ASK
//     ASSERT_EQ(encryptResult, CKR_OK);
    
//     size_t decryptedLength = client.getDecryptedLenClient( senderId,  encryptedLength);
//     uint8_t decryptedData[decryptedLength];
//     CK_RV decryptResult = client.decrypt(senderId, receiverId, encryptedData, encryptedLength, decryptedData, decryptedLength);

//     ASSERT_EQ(decryptResult, CKR_OK);
//     ASSERT_FALSE(memcmp(decryptedData, message, messageLen)) << "Decrypted data does not match original data";
// }

// TEST_F(CryptoClientTest, SignVerify) 
// {
//     char* message = new char[messageLen]; 
//     std::memset(message, 'A', messageLen);
//     size_t signedMessageLen = client.getSignedDataLength(messageLen);
//     uint8_t* signedMessage = new uint8_t[signedMessageLen];
//     SHAAlgorithm hashFunc = SHA_256;
//     auto rsaKeysPair = client.generateRSAKeyPair(receiverId, {KeyPermission::VERIFY, KeyPermission::SIGN, KeyPermission::ENCRYPT, KeyPermission::DECRYPT, KeyPermission::EXPORTABLE});
//     std::string publicKey = client.getPublicRSAKeyByUserId(receiverId);

//     size_t verifiedMessageLen = client.getVerifiedDataLength(signedMessageLen);
//     unsigned char verifiedMessage[verifiedMessageLen];

//     CK_RV signResult = client.sign(senderId, message, messageLen, signedMessage, signedMessageLen, hashFunc, publicKey);
//     ASSERT_EQ(signResult, CKR_OK);

//     CK_RV verifyResult = client.verify(senderId, receiverId, signedMessage, signedMessageLen, verifiedMessage, verifiedMessageLen, rsaKeysPair.second);
//     ASSERT_EQ(verifyResult, CKR_OK);
//     ASSERT_EQ(memcmp(message, verifiedMessage, messageLen),0);
//     delete[] signedMessage;
// }

// TEST_F(CryptoClientTest, RSAEncryptDecrypt) 
// {
//     auto rsaKeyPair = client.generateRSAKeyPair(senderId, {KeyPermission::VERIFY, KeyPermission::SIGN, KeyPermission::ENCRYPT, KeyPermission::DECRYPT, KeyPermission::EXPORTABLE});
//     ASSERT_FALSE(rsaKeyPair.first.empty() || rsaKeyPair.second.empty());

//     const char* inputData = "Hello, World!";
//     size_t inputDataLen = strlen(inputData);
//     size_t encryptedDataLenRSA = client.getRSAencryptedLength();
//     uint8_t* encryptedDataRSA = new uint8_t[encryptedDataLenRSA];

//     CK_RV rsaEncryptResult = client.RSAencrypt(senderId, rsaKeyPair.second, (void*)inputData, inputDataLen, encryptedDataRSA, encryptedDataLenRSA);
//     ASSERT_EQ(rsaEncryptResult, CKR_OK);
//     size_t decryptedDataLenRSA = client.getRSAdecryptedLength();
//     uint8_t* decryptedDataRSA = new uint8_t[decryptedDataLenRSA];
//     std::string publicKey = client.getPublicRSAKeyByUserId(senderId);
//     CK_RV rsaDecryptResult = client.RSAdecrypt(receiverId, publicKey, encryptedDataRSA, encryptedDataLenRSA, decryptedDataRSA, decryptedDataLenRSA);
//     ASSERT_EQ(rsaDecryptResult, CKR_OK);
//     ASSERT_FALSE(memcmp(inputData, decryptedDataRSA, inputDataLen));

//     delete[] encryptedDataRSA;
//     delete[] decryptedDataRSA;
// }


// TEST_F(CryptoClientTest, ECCEncryptDecrypt) 
// {
//     const char* inputData = "Hello, World!";
//     size_t inputDataLen = strlen(inputData);
//     auto eccKey = client.generateECCKeyPair(senderId, {KeyPermission::VERIFY, KeyPermission::SIGN, KeyPermission::ENCRYPT, KeyPermission::DECRYPT, KeyPermission::EXPORTABLE});
//     size_t encryptedDataLenECC = client.getECCencryptedLength();
//     uint8_t encryptedDataECC[encryptedDataLenECC];

//     CK_RV eccEncryptResult = client.ECCencrypt(senderId, eccKey.second, (void*)inputData, inputDataLen, encryptedDataECC, encryptedDataLenECC);
//     ASSERT_EQ(eccEncryptResult, CKR_OK);
    
//     std::string publicKeyId = client.getPublicECCKeyByUserId(senderId);
//     size_t decryptedDataLenECC = client.getECCdecryptedLength();
//     uint8_t decryptedDataECC[inputDataLen];
//     CK_RV eccDecryptResult = client.ECCdecrypt(receiverId, publicKeyId, encryptedDataECC, encryptedDataLenECC, decryptedDataECC, decryptedDataLenECC);
    
//     ASSERT_EQ(eccDecryptResult, CKR_OK);
//     ASSERT_FALSE(memcmp(inputData, decryptedDataECC, inputDataLen));
// }

// TEST_F(CryptoClientTest, AESncryptDecrypt)
// {
//     AESChainingMode mode = ECB;
//     AESKeyLength keyLength = AES_128;
//     char* inputData = new char[messageLen]; 
//     std::memset(inputData, 'A', messageLen);
//     size_t inputDataLen = messageLen;
//     std::vector<KeyPermission> permissions = {
//         KeyPermission::DECRYPT, KeyPermission::ENCRYPT, KeyPermission::SIGN,
//         KeyPermission::VERIFY, KeyPermission::EXPORTABLE};
//     AESChainingMode chainingMode = mode;  
//     std::string keyId = client.generateAESKey(senderId, keyLength, permissions, receiverId);
//     size_t encryptedLength = client.getAESencryptedLengthClient(inputDataLen, mode, keyLength,RSA,keyId);
//     void* encryptedData = new uint8_t[encryptedLength];
//     // Encrypt the data
//     CK_RV result = client.AESencrypt(senderId, receiverId, (void *)inputData, inputDataLen,
//                                encryptedData, reinterpret_cast<unsigned int&>(encryptedLength), RSA,
//                                keyLength, mode, keyId);
//     // Check for successful encryption
//     EXPECT_EQ(result, CKR_OK);

//     // Decrypt the data
//     size_t decryptedLength =
//        client.getAESdecryptedLengthClient(encryptedData, encryptedLength);
//     uint8_t* decryptedData = new uint8_t[inputDataLen];
//     result = client.AESdecrypt(senderId, receiverId,(uint8_t*) encryptedData, encryptedLength,
//     (void*&)decryptedData, decryptedLength);
//     EXPECT_EQ(result, CKR_OK);
//     EXPECT_EQ(memcmp(inputData, decryptedData, inputDataLen), 0);

// }

TEST_F(CryptoClientTest, EncryptDecryptChunk)
{
    // Create the specific chunk of data: 00 00 c8 41 00 00 a0 41
    uint8_t chunk[] = {0x00, 0x00, 0xc8, 0x41, 0x00, 0x00, 0xa0, 0x41};
    size_t chunkLen = sizeof(chunk);

    // Get the encrypted data length based on the input
    size_t encryptedLength = client.getEncryptedLenClient(senderId, chunkLen);
    uint8_t* encryptedData = new uint8_t[encryptedLength];

    // Perform encryption
    CK_RV encryptResult = client.encrypt(senderId, receiverId, (void *)chunk, chunkLen, encryptedData, encryptedLength);
    ASSERT_EQ(encryptResult, CKR_OK);

    // Get the decrypted data length based on the encrypted data
    size_t decryptedLength = client.getDecryptedLenClient(senderId, encryptedLength);
    uint8_t* decryptedData = new uint8_t[decryptedLength];

    // Perform decryption
    CK_RV decryptResult = client.decrypt(senderId, receiverId, encryptedData, encryptedLength, decryptedData, decryptedLength);
    ASSERT_EQ(decryptResult, CKR_OK);

    // Ensure the decrypted data matches the original chunk
    ASSERT_EQ(memcmp(decryptedData, chunk, chunkLen), 0) << "Decrypted data does not match original data";

    // Cleanup
    delete[] encryptedData;
    delete[] decryptedData;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}