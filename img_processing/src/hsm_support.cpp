#include "hsm_support.h"
#include "crypto_api.h"

bool decryptData(void *data, int dataLen, uint32_t senderId, uint32_t myId)
{
    CryptoClient client(myId);
    size_t encryptedLength = client.getEncryptedLen(senderId, dataLen);

    size_t decryptedLength = client.getEncryptedLen(senderId, encryptedLength);

    uint8_t decryptedData[decryptedLength];

    CK_RV decryptResult = client.decrypt(senderId, data, encryptedLength,
                                         decryptedData, decryptedLength);


    if (decryptResult != CKR_OK || decryptedLength != dataLen)
        return false;
    memcpy(data, decryptedData, decryptedLength);
    return true;
}

bool encryptData(const void *data, int dataLen, uint8_t *encryptedData,
                 size_t encryptedLength, uint32_t receiverId, uint32_t myId)
{
    CryptoClient client(myId);
    // Encrypt the data
    CK_RV encryptResult =client.encrypt(
        receiverId, data, dataLen, encryptedData, encryptedLength);
    // Check if encryption was successful
    if (encryptResult != CKR_OK)
        return false;
    return true;
}