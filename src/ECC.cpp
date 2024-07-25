#include "ECC.h"
#include <cmath>
#include <random>
pair<Point, Point> ECC::encrypt(Point message)
{
    Point C1 = multiply(basicPoint, k);
    Point C2 = add(message, multiply(publicKey, k));
    return make_pair(C1, C2);
}
Point ECC::decrypt(pair<Point, Point> ciphertext)
{
    Point temp = multiply(ciphertext.first, privateKey);
    Point decrypted = add(ciphertext.second, Point(temp.x, (prime - temp.y) % prime));
    return decrypted;
}
ECC::ECC(Point basicPoint) : basicPoint(basicPoint)
{
    privateKey = generatePrivateKey();
    publicKey = generatePublicKey();
    k = generateK();
}
bool ECC::isOnCurve(Point P)
{
  return mod(P.y * P.y) == mod(P.x * P.x * P.x + a * P.x + b);
}
Point ECC::add(Point P, Point Q)
{
    long long incline;
    if (P.x == 0 && P.y == 0)
      return Q;
    if (Q.x == 0 && Q.y == 0)
      return P;
    if (P.x == Q.x && P.y == -Q.y)
      return Point(0, 0);
    if (P.x == Q.x && P.y == Q.y)
      incline = mod((3 * P.x * P.x + a) * inverse(2 * P.y));
    else
      incline = mod((Q.y - P.y) * inverse(Q.x - P.x));
    long long x = mod(incline * incline - P.x - Q.x);
    long long y = mod(incline * (P.x - x) - P.y);
    return Point(x, y);
}
Point ECC::multiply(Point P, long long times)
{
    Point R(0, 0);
    Point N = P;
    while (times > 0) {
        if (times % 2 == 1) {
            R = add(R, N);
        }
        N = add(N, N);
        times /= 2;
    }
    return R;
}
long long ECC::mod(long long x)
{
  long long modulo = x % prime;
  return modulo < 0 ? modulo + prime : modulo;
}
long long ECC::inverse(long long base)
{
    long long result = 1;
    long long exp = prime - 2;
    base = mod(base);
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = mod(result * base);
        }
        exp = exp >> 1;
        base = mod(base * base);
    }
    return result;
}
long long ECC::generateK()
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<long long> dis(1, prime-1);
    return dis(gen);
}
long long ECC::generatePrivateKey()
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<long long> dis(1, prime-1);
    return dis(gen);
}
Point ECC::generatePublicKey() {
    return multiply(basicPoint, privateKey);
}













