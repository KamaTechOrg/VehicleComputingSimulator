#ifndef ECC_KEYS_H
#define ECC_KEYS_H

#include <gmpxx.h>  // Include GMP C++ header

#include "asymmetric_keys.h"
//#include "ecc.h"

// Structure for representing a point
struct Point {
    mpz_class x;
    mpz_class y;
    Point() : x(0), y(0) {}
    Point(mpz_class x, mpz_class y) : x(x), y(y) {}
};

class EccKeys : public AsymmetricKeys {
   public:
    mpz_class privateKey;
    Point publicKey;

    // Default constructor
    EccKeys();

    // Parameterized constructor
    EccKeys(const mpz_class &privateKey, const Point &publicKey);

    // Copy constructor
    EccKeys(const EccKeys &other);

    // Destructor
    ~EccKeys();

    // Copy assignment operator
    EccKeys &operator=(const EccKeys &other);
};

#endif  // ECC_KEYS_H