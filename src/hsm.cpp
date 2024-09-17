// #include "../include/hsm.h"

// // Initialize static member
// std::map<int, AsymmetricKeys *> Hsm::keys;

// std::map<int, encryptionFunction> Hsm::encryptionFunctions;

// encryptionFunction Hsm::getEncryptionFunctionType(int id)
// {
//     auto it = encryptionFunctions.find(id);
//     if (it != encryptionFunctions.end()) {
//         return it->second;
//     }
//     else {
//         throw "ID not found";
//     }
// }


// //for initializing proccesses when booting up the system
// void Hsm::addProccessKeyandFuncType(int keyID, encryptionFunction func)
// {
//     if (isRsaEncryption(func))
//         Hsm::keys[keyID] = rsaGenerateKeys(rsaKeySize);
//     else {
//         //TODO
//         //Hsm::keys[keyID]=eccGenerateKeys();
//     }
//     encryptionFunctions[keyID] = func;
// }

// void Hsm::setKey(int keyId, AsymmetricKeys *keys)
// {
//     Hsm::keys[keyId] = keys;
// }

// AsymmetricKeys *Hsm::getKey(int keyId)
// {
//     return keys[keyId];
// }