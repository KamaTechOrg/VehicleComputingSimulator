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

std::string Hsm::generateKeyId() {
    // יצירת ID ייחודי המבוסס על הזמן הנוכחי (או כל שיטה אחרת ליצירת מזהה ייחודי)
    auto now = std::chrono::system_clock::now();
    auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    return "key_" + std::to_string(nowMs);
}

void Hsm::updateACL(int userId, const std::string& fileName, const std::vector<KeyPermission>& permissions, const std::string& keyId, const std::string& keyType) {
    std::string aclFile = "../keys/keys.json";
    nlohmann::json aclData;

    // קריאת קובץ ה-ACL הקיים
    std::ifstream aclInput(aclFile);
    if (aclInput.is_open()) {
        aclInput >> aclData;
        aclInput.close();
    }

    // יצירת אובייקט ACL חדש
    nlohmann::json newACL;
    newACL["user"] = userId;
    newACL["key_type"] = keyType;
    newACL["file_name"] = fileName;
    newACL["key_id"] = keyId;

    // הוספת הרשאות למערך ה-ACL
    nlohmann::json permissionsArray = nlohmann::json::array();
    for (const auto& permission : permissions) {
        nlohmann::json permissionEntry;
        permissionEntry["encrypt"] = (permission == KeyPermission::ENCRYPT);
        permissionEntry["sign"] = (permission == KeyPermission::SIGN);
        permissionEntry["verify"] = (permission == KeyPermission::VERIFY);
        permissionsArray.push_back(permissionEntry);
    }

    newACL["permissions"] = permissionsArray;

    // הוספת ה-ACL החדש לקובץ
    aclData["keys"].push_back(newACL);

    // שמירת ה-ACL לקובץ
    std::ofstream aclOutput(aclFile);
    if (aclOutput.is_open()) {
        aclOutput << aclData.dump(4); // שמירה עם הזחות
        aclOutput.close();
    } else {
        std::cerr << "Error: Could not open ACL file to write." << std::endl;
    }
}


std::pair<std::string, std::string> Hsm::generateECCKeyPair(int userId, std::vector<KeyPermission> permissions) {
    // יצירת מזהים ייחודיים למפתחות
    std::string privateKeyId = generateKeyId();
    std::string publicKeyId = generateKeyId();

    std::string privateKeyFile = "../keys/" + privateKeyId + ".key";
    std::string publicKeyFile = "../keys/" + publicKeyId + ".pub";

    // יצירת מפתח פרטי
    mpz_class privateKey = generatePrivateKey();

    // שמירת מפתח פרטי בקובץ
    std::ofstream privFile(privateKeyFile);
    if (privFile.is_open()) {
        privFile << privateKey.get_str(16);
        privFile.close();
        std::cout << "ECC private key saved to file: " << privateKeyFile << std::endl;
    } else {
        std::cerr << "Error: Could not open file " << privateKeyFile << " to write." << std::endl;
        return {"", ""};
    }

    // יצירת מפתח ציבורי ושמירתו בקובץ
    Point publicKey = generatePublicKey(privateKey);
    std::ofstream pubFile(publicKeyFile);
    if (pubFile.is_open()) {
        pubFile << "x: " << publicKey.x.get_str(16) << std::endl;
        pubFile << "y: " << publicKey.y.get_str(16) << std::endl;
        pubFile.close();
        std::cout << "ECC public key saved to file: " << publicKeyFile << std::endl;
    } else {
        std::cerr << "Error: Could not open file " << publicKeyFile << " to write." << std::endl;
        return {"", ""};
    }

    // עדכון ה-ACL עם ההרשאות
    updateACL(userId, privateKeyFile, permissions, privateKeyId, "ECC-Private");
    updateACL(userId, publicKeyFile, permissions, publicKeyId, "ECC-Public");

    // החזרת ה-IDs שנוצרו למפתחות
    return {privateKeyId, publicKeyId};
}


std::string Hsm::generateAESKey(int userId, std::vector<KeyPermission> permissions) {
    // יצירת מזהה ייחודי למפתח
    std::string keyId = generateKeyId();
    std::string keyFileName = "../keys/" + keyId + ".key";

    // יצירת המפתח באמצעות הפונקציה
    unsigned char * generatedKey = generateKey(AESKeyLength::AES_128);

    // שמירת המפתח בקובץ
    std::ofstream keyFile(keyFileName);
    if (keyFile.is_open()) {
        keyFile << generatedKey;
        keyFile.close();
        std::cout << "AES key saved to file: " << keyFileName << std::endl;
    } else {
        std::cerr << "Error: Could not open file " << keyFileName << " to write." << std::endl;
        return "";
    }

    // עדכון ה-ACL עם ההרשאות
    updateACL(userId, keyFileName, permissions, keyId, "AES");

    // החזרת ה-ID שנוצר למפתח
    return keyId;
}


// std::pair<std::string, std::string> Hsm::generateRSAKeyPair(int userId, std::vector<KeyPermission> permissions) {
//     // יצירת מזהים ייחודיים למפתחות
//     std::string privateKeyId = generateKeyId();
//     std::string publicKeyId = generateKeyId();
    
//     std::string privateKeyFile = "../keys/" + privateKeyId + ".key";
//     std::string publicKeyFile = "../keys/" + publicKeyId + ".pub";

//     // יצירת מפתח פרטי
//     mpz_class privateKey = generatePrivateKey();

//     // שמירת מפתח פרטי בקובץ
//     std::ofstream privFile(privateKeyFile);
//     if (privFile.is_open()) {
//         privFile << privateKey.get_str(16);
//         privFile.close();
//         std::cout << "RSA private key saved to file: " << privateKeyFile << std::endl;
//     } else {
//         std::cerr << "Error: Could not open file " << privateKeyFile << " to write." << std::endl;
//         return {"", ""};
//     }

//     // יצירת מפתח ציבורי ושמירתו בקובץ
//     Point publicKey = generatePublicKey(privateKey);
//     std::ofstream pubFile(publicKeyFile);
//     if (pubFile.is_open()) {
//         pubFile << "x: " << publicKey.x.get_str(16) << std::endl;
//         pubFile << "y: " << publicKey.y.get_str(16) << std::endl;
//         pubFile.close();
//         std::cout << "RSA public key saved to file: " << publicKeyFile << std::endl;
//     } else {
//         std::cerr << "Error: Could not open file " << publicKeyFile << " to write." << std::endl;
//         return {"", ""};
//     }

//     // עדכון ה-ACL עם ההרשאות
//     updateACL(userId, privateKeyFile, permissions, privateKeyId, "RSA-Private");
//     updateACL(userId, publicKeyFile, permissions, publicKeyId, "RSA-Public");

//     // החזרת ה-IDs שנוצרו למפתחות
//     return {privateKeyId, publicKeyId};
// }



bool Hsm::isPermission(int keyId, int userId, KeyPermission permission) {
    std::string aclFileName = "../keys/keys.json";
    std::ifstream aclFile(aclFileName);
    if (!aclFile.is_open()) {
        std::cerr << "ACL file does not exist" << std::endl;
        return false;
    }

    nlohmann::json aclJson;
    try {
        aclFile >> aclJson;
    } catch (const std::exception &e) {
        std::cerr << "Error reading ACL JSON: " << e.what() << std::endl;
        return false;
    }

    std::string keyIdStr = std::to_string(keyId);
    auto keys = aclJson["keys"];
    
    // חיפוש המפתח לפי keyId
    for (const auto& keyEntry : keys) {
        if (keyEntry["key_id"] == keyIdStr) {
            auto aclArray = keyEntry["ACL"];
            // חיפוש המשתמש בהרשאות של המפתח
            for (const auto& userEntry : aclArray) {
                if (userEntry["user"] == std::to_string(userId)) {
                    // בדיקת ההרשאה
                    std::string permissionStr;
                    switch (permission) {
                        case KeyPermission::ENCRYPT: permissionStr = "encrypt"; break;
                        case KeyPermission::SIGN: permissionStr = "sign"; break;
                        case KeyPermission::VERIFY: permissionStr = "verify"; break;
                        case KeyPermission::EXPORTABLE: permissionStr = "exportable"; break;
                        case KeyPermission::DECRYPT: permissionStr = "decrypt"; break;
                    }

                    if (userEntry[permissionStr] == true) {
                        return true; // ההרשאה קיימת
                    }
                }
            }
        }
    }

    std::cerr << "Permission not found" << std::endl;
    return false;
}

std::string Hsm::readFileContent(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

// הפונקציה בודקת אם למשתמש יש הרשאה למפתח ושולפת את המפתח אם כן
std::string Hsm::getAuthorizedKey(int userId,std::string& keyId, std::string& permission) {
    // קריאת קובץ ה-ACL
    std::ifstream aclFile("../keys/acl.json");
    if (!aclFile.is_open()) {
        throw std::runtime_error("Failed to open ACL file.");
    }
    
    nlohmann::json aclData;
    aclFile >> aclData;

    // חיפוש המפתח והרשאה עבור המשתמש
    for (const auto& entry : aclData["keys"]) {
        if (entry["user"] == userId && entry["key_id"] == keyId) {
            for (const auto& perm : entry["permissions"]) {
                if (perm.contains(permission) && perm[permission].get<bool>() == true) {
                    // הרשאה קיימת - שליפת המפתח
                    std::string keyFilePath = "../keys/" + keyId + ".key";
                    if (std::filesystem::exists(keyFilePath)) {
                        return readFileContent(keyFilePath);
                    } else {
                        throw std::runtime_error("Key file does not exist: " + keyFilePath);
                    }
                }
            }
        }
    }

    // אם ההרשאה לא נמצאה
    throw std::runtime_error("Permission not found or not granted for this user and key.");
}
// void Hsm::saveKey(int keyId, const std::string& keyData, bool isPublic) {
//     std::string fileName = "../keys/key_" + std::to_string(keyId);
//     fileName += isPublic ? ".pub" : ".key";

//     std::ofstream keyFile(fileName);
//     if (!keyFile.is_open()) {
//         std::cerr << "Error opening key file for writing: " << fileName << std::endl;
//         return;
//     }

//     keyFile << keyData;
//     keyFile.close();
// }
// void Hsm::addKeyToAcl(int keyId, const std::string& fileName, const nlohmann::json& acl) {
//     std::string aclFileName = "../keys/keys.json";
//     std::ifstream aclFile(aclFileName);
//     nlohmann::json aclJson;

//     if (aclFile.is_open()) {
//         aclFile >> aclJson;
//         aclFile.close();
//     }

//     nlohmann::json newKeyEntry = {
//         {"key_id", std::to_string(keyId)},
//         {"file_name", fileName},
//         {"ACL", acl}
//     };

//     aclJson["keys"].push_back(newKeyEntry);

//     std::ofstream outFile(aclFileName);
//     if (!outFile.is_open()) {
//         std::cerr << "Error opening ACL file for writing: " << aclFileName << std::endl;
//         return;
//     }

//     outFile << aclJson.dump(4);  // שמירת הקובץ עם פיסוק
//     outFile.close();
// }

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