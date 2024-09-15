// #include "../include/hsm.h"
// // #include "rsa.h"
// #include <fstream>
// #include <stdexcept>

// // Initialize static member
// std::map<int, AsymmetricKeys *> Hsm::keys;
// std::map<int, encryptionFunction> Hsm::encryptionFunctions;
// std::map<int, std::string> Hsm::permissions;

// // Function to get the encryption function type by ID
// encryptionFunction Hsm::getEncryptionFunctionType(int id) {
//     auto it = encryptionFunctions.find(id);
//     if (it != encryptionFunctions.end()) {
//         return it->second;
//     } else {
//         throw std::runtime_error("ID not found");
//     }
// }

// // Initialize process key and function type
// void Hsm::addProccessKeyandFuncType(int keyID, encryptionFunction func) {
//     if (isRsaEncryption(func)) {
//         // Hsm::keys[keyID] = rsaGenerateKeys(rsaKeySize);
//     } else {
//         // TODO: Implement ECC key generation
//         // Hsm::keys[keyID] = eccGenerateKeys();
//     }
//     encryptionFunctions[keyID] = func;
// }

// // Set a key in the system
// void Hsm::setKey(int keyId, AsymmetricKeys *keys) {
//     Hsm::keys[keyId] = keys;
// }

// // Get a key from the system
// AsymmetricKeys *Hsm::getKey(int keyId) {
//     auto it = Hsm::keys.find(keyId);
//     if (it != Hsm::keys.end()) {
//         return it->second;
//     } else {
//         throw std::runtime_error("Key ID not found");
//     }
// }

// // Save key to a file
// void Hsm::saveKeyToFile(int keyId, const std::string &filename) {
//     auto it = Hsm::keys.find(keyId);
//     if (it != Hsm::keys.end()) {
//         std::ofstream outFile(filename, std::ios::binary);
//         if (outFile.is_open()) {
//             it->second->serialize(outFile);  // Assuming AsymmetricKeys has a serialize method
//             outFile.close();
//         } else {
//             throw std::runtime_error("Unable to open file for writing");
//         }
//     } else {
//         throw std::runtime_error("Key ID not found");
//     }
// }

// // Load key from a file
// void Hsm::loadKeyFromFile(int keyId, const std::string &filename) {
//     auto it = Hsm::keys.find(keyId);
//     if (it != Hsm::keys.end()) {
//         std::ifstream inFile(filename, std::ios::binary);
//         if (inFile.is_open()) {
//             it->second->deserialize(inFile);  // Assuming AsymmetricKeys has a deserialize method
//             inFile.close();
//         } else {
//             throw std::runtime_error("Unable to open file for reading");
//         }
//     } else {
//         throw std::runtime_error("Key ID not found");
//     }
// }

// // Set permissions for a key
// void Hsm::setPermissions(int keyId, const std::string &permissionsFile) {
//     std::ifstream inFile(permissionsFile);
//     if (inFile.is_open()) {
//         std::string perms((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
//         permissions[keyId] = perms;
//         inFile.close();
//     } else {
//         throw std::runtime_error("Unable to open permissions file");
//     }
// }

// // Get permissions for a key
// std::string Hsm::getPermissions(int keyId) {
//     auto it = permissions.find(keyId);
//     if (it != permissions.end()) {
//         return it->second;
//     } else {
//         throw std::runtime_error("Permissions for Key ID not found");
//     }
// }

// // Check if the encryption function uses RSA
// bool Hsm::isRsaEncryption(encryptionFunction func) {
//     // Example check - customize based on your actual encryption function types
//     return func >= SIGNATURE_SHA256_CBC && func <= ENCRYPT_256_AND_SIGN_SHA3_512_OFB;
// }

// void Hsm::saveKeyWithPermissionToFile(int keyID, const std::string& fileName)
// {
//     auto keyIt = Hsm::keys.find(keyID);
//     if (keyIt == Hsm::keys.end()) {
//         std::cerr << "Error: Key with ID " << keyID << " not found." << std::endl;
//         return;
//     }

//     auto metaDataIt = Hsm::keyMetaData.find(keyID);
//     if (metaDataIt == Hsm::keyMetaData.end()) {
//         std::cerr << "Error: Metadata for key with ID " << keyID << " not found." << std::endl;
//         return;
//     }

//     std::ofstream outFile(fileName);
//     if (!outFile) {
//         std::cerr << "Error: Could not open file " << fileName << " for writing." << std::endl;
//         return;
//     }

//     outFile << "Key ID: " << keyID << std::endl;
//     outFile << "Permissions:" << std::endl;
//     const KeyMetaData& metaData = metaDataIt->second;
//     for (KeyPermission perm : metaData.permissions) {
//         switch (perm) {
//             case PERMISSION_ENCRYPT:
//                 outFile << "- Encrypt" << std::endl;
//                 break;
//             case PERMISSION_DECRYPT:
//                 outFile << "- Decrypt" << std::endl;
//                 break;
//             case PERMISSION_SIGN:
//                 outFile << "- Sign" << std::endl;
//                 break;
//             case PERMISSION_VERIFY:
//                 outFile << "- Verify" << std::endl;
//                 break;
//         }
//     }
//     outFile.close();
// }
#include <fstream>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include "../include/hsm.h"
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>  // הספרייה nlohmann::json

// using json = nlohmann::json;

// Initialize static members
std::map<int, AsymmetricKeys *> Hsm::keys;
std::map<int, encryptionFunction> Hsm::encryptionFunctions;
std::map<int, KeyMetaData> Hsm::keyMetaData;

// encryptionFunction Hsm::getEncryptionFunctionType(int id)
// {
//     auto it = encryptionFunctions.find(id);
//     if (it != encryptionFunctions.end()) {
//         return it->second;
//     } else {
//         throw "ID not found";
//     }
// }

// // Initialize processes when booting up the system
// void Hsm::addKey(int keyID, AsymmetricKeys *keys, encryptionFunction func, const KeyMetaData& metaData)
// {
//     if (Hsm::keys.find(keyID) != Hsm::keys.end()) {
//         std::cerr << "Error: Key with ID " << keyID << " already exists." << std::endl;
//         return;
//     }

//     Hsm::keys[keyID] = keys;
//     Hsm::encryptionFunctions[keyID] = func;
//     Hsm::keyMetaData[keyID] = metaData;
// }

// bool Hsm::hasPermission(int keyID, KeyPermission permission)
// {
//     auto it = Hsm::keyMetaData.find(keyID);
//     if (it != Hsm::keyMetaData.end()) {
//         return it->second.permissions.count(permission) > 0;
//     }
//     return false;
// }

// void Hsm::setKey(int keyId, AsymmetricKeys *keys)
// {
//     Hsm::keys[keyId] = keys;
// }

// AsymmetricKeys *Hsm::getKey(int keyId)
// {
//     return keys[keyId];
// }

// bool Hsm::isRsaEncryption(encryptionFunction func)
// {
//     // Example: return true if func involves RSA encryption.
//     return func >= SIGNATURE_SHA256_CBC && func <= ENCRYPT_256_AND_SIGN_SHA256_OFB;
// }

// void Hsm::saveKeyWithPermissionToFile(int keyID, const std::string& fileName)
// {
//     auto keyIt = Hsm::keys.find(keyID);
//     if (keyIt == Hsm::keys.end()) {
//         std::cerr << "Error: Key with ID " << keyID << " not found." << std::endl;
//         return;
//     }

//     auto metaDataIt = Hsm::keyMetaData.find(keyID);
//     if (metaDataIt == Hsm::keyMetaData.end()) {
//         std::cerr << "Error: Metadata for key with ID " << keyID << " not found." << std::endl;
//         return;
//     }

//     std::ofstream outFile(fileName);
//     if (!outFile) {
//         std::cerr << "Error: Could not open file " << fileName << " for writing." << std::endl;
//         return;
//     }

//     outFile << "Key ID: " << keyID << std::endl;
//     outFile << "Permissions:" << std::endl;
//     const KeyMetaData& metaData = metaDataIt->second;
//     for (KeyPermission perm : metaData.permissions) {
//         switch (perm) {
//             case READ:
//                 outFile << "- READ" << std::endl;
//                 break;
//             case WRITE:
//                 outFile << "- WRITE" << std::endl;
//                 break;
//             case EXECUTE:
//                 outFile << "- EXECUTE" << std::endl;
//                 break;

//         }
//     }
//     outFile.close();
// }
// static void generateKey(int userId, KeyPermission permission, std::string (*func)()) {
//     // יצירת תיקיית keys אם היא לא קיימת
//     std::filesystem::create_directory("keys");

//     // יצירת שם הקובץ לפי ה-userId
//     std::string fileName = "keys/key_" + std::to_string(userId) + ".json";

//     // יצירת המפתח באמצעות פונקציית ההצפנה
//     std::string generatedKey = func();

//     // יצירת אובייקט JSON
//     json j;
//     j["userId"] = userId;
//     j["permission"] = (permission == KeyPermission::READ) ? "READ" :
//                       (permission == KeyPermission::WRITE) ? "WRITE" :
//                       "EXECUTE";
//     j["key"] = generatedKey;

//     // שמירת הנתונים לקובץ JSON
//     std::ofstream file(fileName);
//     if (file.is_open()) {
//         file << j.dump(4); // פורמט עם הזחות יפות (indentation) של 4 רמות
//         file.close();
//     } else {
//         std::cerr << "Error: Could not open file to write.\n";
//     }
// }
// void generateKey(int userId, KeyPermission permission, std::string (*func)(int)) {
//     nlohmann::json j;
//     j["userId"] = userId;
//     j["permission"] = (permission == KeyPermission::READ) ? "Read" : "Write";
//     j["key"] = func(userId);

//     std::filesystem::create_directory("keys");

//     std::ofstream file("keys/key_" + std::to_string(userId) + ".json");
//     file << j.dump(4);
//     file.close();
// }
 void Hsm::generateKey(int userId, KeyPermission permission, std::string (*func)(int)) {
    // בדוק אם התיקיה קיימת ואם לא צור אותה
    if (!std::filesystem::exists("keys")) {
        std::cout << "Creating directory '../keys'..." << std::endl;
        if (!std::filesystem::create_directory("keys")) {
            std::cerr << "Error: Could not create directory '../keys'." << std::endl;
            return;
        } else {
            std::cout << "Directory 'keys' created successfully." << std::endl;
        }
    } else {
        std::cout << "Directory 'keys' already exists." << std::endl;
    }

    // יצירת שם הקובץ לפי ה-userId
    std::string fileName = "../keys/key_" + std::to_string(userId) + ".json";
    std::cout << "Creating file: " << fileName << std::endl;

    // יצירת המפתח באמצעות פונקציית ההצפנה
    std::string generatedKey = func(userId);

    // יצירת אובייקט JSON
    nlohmann::json j;
    j["userId"] = userId;
    j["permission"] = (permission == KeyPermission::READ) ? "Read" :
                      (permission == KeyPermission::WRITE) ? "Write" :
                      "Execute";
    j["key"] = generatedKey;

    // שמירת הנתונים לקובץ JSON
    std::ofstream file(fileName);
    if (file.is_open()) {
        file << j.dump(4); // פורמט עם הזחות יפות (indentation) של 4 רמות
        file.close();
        std::cout << "File created and written successfully." << std::endl;
    } else {
        std::cerr << "Error: Could not open file " << fileName << " to write." << std::endl;
    }
}