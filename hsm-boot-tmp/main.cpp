#include "crypto_api.h"
int main()
{
    uint32_t destID = 1;
    uint32_t srcID = 2;
    CryptoClient client1(srcID);
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_128,
                        AESChainingMode::ECB, AsymmetricFunction::RSA);
    client1.configure(config);
    CryptoClient client2(destID);
    client2.configure(config);
    client1.bootSystem(
        {{srcID,
          {KeyPermission::VERIFY, KeyPermission::SIGN, KeyPermission::ENCRYPT,
           KeyPermission::DECRYPT, KeyPermission::EXPORTABLE}},
         {destID,
          {KeyPermission::VERIFY, KeyPermission::SIGN, KeyPermission::ENCRYPT,
           KeyPermission::DECRYPT, KeyPermission::EXPORTABLE}}});
}