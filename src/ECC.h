#ifndef ECC_H_
#define ECC_H_
#include <utility>
using namespace std;
struct Point {
    long long x;
    long long y;
    Point() : x(0), y(0) {}
    Point(long long x, long long y) : x(x), y(y) {}
};
class ECC
{
  private:
    long long privateKey;
    long long generatePrivateKey(); 
    Point generatePublicKey();
    static const long long prime = 17;
    static const long long a = 2; 
    static const long long b = 2; 
  public:
    Point basicPoint;
    Point publicKey;
    long long k;
    ECC(Point basicPoint);
    pair<Point, Point> encrypt(Point message);
    Point decrypt(pair<Point, Point> ciphertext);
    long long generateK(); 
    Point multiply(Point P, long long times);
    Point add(Point P, Point Q);
    long long mod(long long x);
    bool isOnCurve(Point P);
    long long inverse(long long base);
};
#endif