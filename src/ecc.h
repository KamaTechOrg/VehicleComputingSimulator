#pragma once

#include <string>
#include <gmpxx.h>

#ifdef USE_SYCL

// Structure for representing a point
struct Point {
    mpz_class x;
    mpz_class y;
    Point() : x(0), y(0) {}
    Point(mpz_class x, mpz_class y) : x(x), y(y) {}
};

// Structure for representing an encrypted message
struct EncryptedMessage {
    mpz_class c1X;
    bool c1Y;
    mpz_class c2X;
    bool c2Y;
    EncryptedMessage(mpz_class c1X, bool c1Y, mpz_class c2X, bool c2Y)
        : c1X(c1X), c1Y(c1Y), c2X(c2X), c2Y(c2Y)
    {
    }
};

void init();
mpz_class generatePrivateKey();
Point generatePublicKey();
mpz_class calculateY(mpz_class x);
Point convertMessageToPoint(const std::string &text);
std::string convertPointToMessage(const Point &point);
EncryptedMessage encryptECC(std::string message, Point key);
std::string decryptECC(EncryptedMessage, mpz_class key);
mpz_class generateK();
Point multiply(Point P, mpz_class times);
Point add(Point P, Point Q);
mpz_class mod(mpz_class x);
bool isOnCurve(Point P);
mpz_class inverse(mpz_class base);
bool modularSqrt(mpz_t result, const mpz_t a, const mpz_t p);

#else  // ECC_NO_SYCL

// Structure for representing a point
struct Point {
    mpz_class x;
    mpz_class y;
    Point() : x(0), y(0) {}
    Point(mpz_class x, mpz_class y) : x(x), y(y) {}
};

// Structure for representing an encrypted message
struct EncryptedMessage {
    mpz_class c1X;
    bool c1Y;
    mpz_class c2X;
    bool c2Y;
    EncryptedMessage(mpz_class c1X, bool c1Y, mpz_class c2X, bool c2Y)
        : c1X(c1X), c1Y(c1Y), c2X(c2X), c2Y(c2Y)
    {
    }
};

mpz_class generatePrivateKey();
Point generatePublicKey(mpz_class key);
mpz_class calculateY(mpz_class x);
Point convertMessageToPoint(const std::string& text);
std::string convertPointToMessage(const Point &point);
EncryptedMessage encryptECC(std::string message, Point key);
std::string decryptECC(EncryptedMessage, mpz_class key);
mpz_class generateK();
Point multiply(Point P, mpz_class times);
Point add(Point P, Point Q);
mpz_class mod(mpz_class x);
bool isOnCurve(Point P);
mpz_class inverse(mpz_class base);
std::pair<mpz_class, mpz_class> signMessageECC(const std::vector<uint8_t>& hash, mpz_class privateKey);
bool verifySignatureECC(const std::vector<uint8_t>& hash, const mpz_class &r, const mpz_class &s, Point publicKey) ;
bool modularSqrt(mpz_t result, const mpz_t a, const mpz_t p);

#endif  // USE_SYCL
