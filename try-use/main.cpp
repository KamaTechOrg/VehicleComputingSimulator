#include "../hsm-client/include/crypto_api.h" // Adjust this include based on your headers
#include <iostream>

int main() {
  CryptoClient client;
  int senderId = 1;
  int receiverId = 2;
  size_t messageLen = 2590;

  // Configure the client
  CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_128,
                      AESChainingMode::ECB, AsymmetricFunction::RSA);
  client.configure(senderId, config);
  client.configure(receiverId, config);

  // Boot the system with permissions
  client.bootSystem(
      {{senderId,
        {KeyPermission::VERIFY, KeyPermission::SIGN, KeyPermission::ENCRYPT,
         KeyPermission::DECRYPT, KeyPermission::EXPORTABLE}},
       {receiverId,
        {KeyPermission::VERIFY, KeyPermission::SIGN, KeyPermission::ENCRYPT,
         KeyPermission::DECRYPT, KeyPermission::EXPORTABLE}}});

  // Prepare a message for encryption
  char *message = new char[messageLen];
  std::memset(message, 'A', messageLen);

  // Encrypt the message
  size_t encryptedLength = client.getEncryptedLenClient(senderId, messageLen);
  uint8_t *encryptedData = new uint8_t[encryptedLength];

  CK_RV encryptResult =
      client.encrypt(senderId, receiverId, (void *)message, messageLen,
                     encryptedData, encryptedLength);

  if (encryptResult != CKR_OK) {
    std::cerr << "Encryption failed!" << std::endl;
    delete[] message;
    delete[] encryptedData;
    return -1;
  }

  std::cout << "Message encrypted successfully!" << std::endl;

  // Decrypt the message
  size_t decryptedLength =
      client.getDecryptedLenClient(senderId, encryptedLength);
  uint8_t *decryptedData = new uint8_t[decryptedLength];

  CK_RV decryptResult =
      client.decrypt(senderId, receiverId, encryptedData, encryptedLength,
                     decryptedData, decryptedLength);

  if (decryptResult != CKR_OK) {
    std::cerr << "Decryption failed!" << std::endl;
    delete[] message;
    delete[] encryptedData;
    delete[] decryptedData;
    return -1;
  }

  std::cout << "Message decrypted successfully!" << std::endl;

  // Verify decrypted data
  if (memcmp(decryptedData, message, messageLen) == 0) {
    std::cout << "Decrypted data matches the original!" << std::endl;
  } else {
    std::cerr << "Decrypted data does not match the original!" << std::endl;
  }

  // Clean up
  delete[] message;
  delete[] encryptedData;
  delete[] decryptedData;

  return 0;
  
}
