#include "hsm_support.h"

bool decryptData(void *data, int dataLen, uint32_t senderId)
{
    GlobalProperties &instanceGP = GlobalProperties::getInstance();
cout << "----0----" << endl;
    size_t encryptedLength =
        instanceGP.client.getEncryptedLen(senderId, dataLen);cout << "----1----" << endl;

    size_t decryptedLength =
        instanceGP.client.getEncryptedLen(senderId, encryptedLength);cout << "----2----" << endl;


    uint8_t decryptedData[decryptedLength];

    CK_RV decryptResult = instanceGP.client.decrypt(
        senderId, data, encryptedLength, decryptedData, decryptedLength);
cout << "-----3---" << endl;

    if (decryptResult != CKR_OK || decryptedLength != dataLen)
        return false;
    memcpy(data, decryptedData, decryptedLength);cout << "---4----" << endl;

    return true;
}

bool encryptData(const void *data, int dataLen, uint8_t *encryptedData,
                 size_t encryptedLength, uint32_t receiverId)
{
    GlobalProperties &instanceGP = GlobalProperties::getInstance();

    // Encrypt the data
    CK_RV encryptResult = instanceGP.client.encrypt(
        receiverId, data, dataLen, encryptedData, encryptedLength);
    // Check if encryption was successful
    if (encryptResult != CKR_OK)
        return false;
    return true;
}