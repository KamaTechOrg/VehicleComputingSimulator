#include "hsm_support.h"

bool decryptData(void *data, int dataLen, uint32_t senderId, uint32_t myId)
{
    GlobalProperties &instanceGP = GlobalProperties::getInstance();

    size_t encryptedLength =
        instanceGP.client.getEncryptedLenClient(senderId, dataLen);
    size_t decryptedLength =
        instanceGP.client.getDecryptedLenClient(senderId, encryptedLength);

    uint8_t decryptedData[decryptedLength];

    CK_RV decryptResult = instanceGP.client.decrypt(
        senderId, myId, data, encryptedLength, decryptedData, decryptedLength);

    if (decryptResult != CKR_OK || decryptedLength != dataLen)
        return false;
    memcpy(data, decryptedData, decryptedLength);
    return true;
}


bool encryptData(const void *data, int dataLen, uint8_t *encryptedData,
                 size_t encryptedLength, uint32_t receiverId, uint32_t myId)
{
    GlobalProperties &instanceGP = GlobalProperties::getInstance();

    // Encrypt the data
    CK_RV encryptResult = instanceGP.client.encrypt(
        myId, receiverId, data, dataLen, encryptedData, encryptedLength);
    // Check if encryption was successful
    if (encryptResult != CKR_OK)
        return false;
    return true;
}