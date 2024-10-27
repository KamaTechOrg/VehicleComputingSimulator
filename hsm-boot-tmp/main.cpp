#include "crypto_api.h"
#include <iostream>
#include <cstdlib>  // For std::atoi

int main(int argc, char* argv[]) {
    // Check for the correct number of arguments
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <srcID> <destID>\n";
        return 1;  // Exit with an error code
    }

    // Convert arguments to uint32_t
    uint32_t srcID = std::atoi(argv[1]);
    uint32_t destID = std::atoi(argv[2]);

    // Create and configure the CryptoClient
    CryptoClient client1(srcID);
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_128,
                        AESChainingMode::ECB, AsymmetricFunction::RSA);
    client1.configure(config);

    // Create the second CryptoClient
    CryptoClient client2(destID);
    client2.configure(config);

    // Boot the system with permissions
    client1.bootSystem(
        {{srcID,
          {KeyPermission::VERIFY, KeyPermission::SIGN, KeyPermission::ENCRYPT,
           KeyPermission::DECRYPT, KeyPermission::EXPORTABLE}},
         {destID,
          {KeyPermission::VERIFY, KeyPermission::SIGN, KeyPermission::ENCRYPT,
           KeyPermission::DECRYPT, KeyPermission::EXPORTABLE}}});

    return 0;  // Exit successfully
}
