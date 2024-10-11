#include "crypto_api.h"
int main(){
     uint32_t destID = 1;
  uint32_t srcID = 2;
    CryptoClient client;
  CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_128, AESChainingMode::ECB, AsymmetricFunction::RSA);
  client.configure(srcID, config);
  client.configure(destID, config);
  client.bootSystem({{srcID, {KeyPermission::VERIFY, KeyPermission::SIGN, KeyPermission::ENCRYPT, KeyPermission::DECRYPT, KeyPermission::EXPORTABLE}},
                      {destID, {KeyPermission::VERIFY, KeyPermission::SIGN, KeyPermission::ENCRYPT, KeyPermission::DECRYPT, KeyPermission::EXPORTABLE}}});


}