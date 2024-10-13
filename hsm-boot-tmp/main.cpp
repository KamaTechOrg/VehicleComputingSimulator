#include "crypto_api.h"
int main()
{
    uint32_t destID = 1;
    uint32_t srcID = 2;
    CryptoClient client(2);
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_128,
                        AESChainingMode::ECB, AsymmetricFunction::RSA);
    client.configure(config);
    client.configure(config);
    client.bootSystem(
        {{srcID,
          {KeyPermission::VERIFY, KeyPermission::SIGN, KeyPermission::ENCRYPT,
           KeyPermission::DECRYPT, KeyPermission::EXPORTABLE}},
         {destID,
          {KeyPermission::VERIFY, KeyPermission::SIGN, KeyPermission::ENCRYPT,
           KeyPermission::DECRYPT, KeyPermission::EXPORTABLE}}});
}