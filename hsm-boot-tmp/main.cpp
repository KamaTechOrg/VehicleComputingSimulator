#include "crypto_api.h"
#include <iostream>
#include <vector>
#include <cstdlib>  // For std::atoi

#include <iostream>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include "hsm_support.h"

int main(int argc, char *argv[])
{
    // Check that there are at least two user IDs provided
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0]
                  << " <userID1> <userID2> [<userID3> ...]\n";
        return 1;  // Exit with an error code
    }

    // Store the user IDs in a vector
    std::vector<uint32_t> userIds;
    for (int i = 1; i < argc; ++i) {
        uint32_t userId = static_cast<uint32_t>(std::atoi(argv[i]));
        userIds.push_back(userId);
    }

    // Check that we have at least two user IDs
    if (userIds.size() < 2) {
        std::cerr << "Error: You must provide at least two user IDs.\n";
        return 1;
    }

    // Example configuration (adjust as needed)
    CryptoConfig config(SHAAlgorithm::SHA_256, AESKeyLength::AES_128,
                        AESChainingMode::ECB, AsymmetricFunction::RSA);

    // Set permissions for these users
    std::vector<KeyPermission> permissions = {
        KeyPermission::VERIFY, KeyPermission::SIGN, KeyPermission::ENCRYPT,
        KeyPermission::DECRYPT, KeyPermission::EXPORTABLE};

    std::map<uint32_t, CryptoConfig> configsMap;
    std::map<uint32_t, std::vector<KeyPermission>> permissionsMap;
    for (const auto &userId : userIds) {
        configsMap[userId] = config;
        permissionsMap[userId] = permissions;
    }
    hsm::configureUsers(configsMap);
    hsm::generateKeys(permissionsMap);

    return 0;  // Exit successfully
}
