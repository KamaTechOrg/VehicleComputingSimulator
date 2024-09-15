// #include "ecc_keys.h"
// // Default constructor
// EccKeys::EccKeys() : privateKey(0), publicKey() {}

// // Parameterized constructor
// EccKeys::EccKeys(const mpz_class &privateKey, const Point &publicKey)
//     : privateKey(privateKey), publicKey(publicKey) {}

// // Copy constructor
// EccKeys::EccKeys(const EccKeys &other) : privateKey(other.privateKey), publicKey(other.publicKey) {}

// // Destructor
// EccKeys::~EccKeys() {}

// // Copy assignment operator
// EccKeys &EccKeys::operator=(const EccKeys &other) {
//     if (this != &other) {
//         privateKey = other.privateKey;
//         publicKey = other.publicKey;
//     }
//     return *this;
// }