#include <iostream>
#include <cstdint>
#include <cmath>
#include <random>

#include "math_utils.h"


using namespace std;


bool isPrime(int64_t number) {
    if (number == 2) return true;
    if (number < 2 || number % 2 == 0) return false;
    
    int64_t limit = static_cast<int64_t>(sqrt(number));
    for (int64_t i = 3; i <= limit; i += 2)
        if (number % i == 0)
            return false;
    return true;
}


int64_t powModFerma(int64_t base, int64_t power, int64_t primeModulo) {
    int64_t reducedPow = power % (primeModulo - 1);

    cout << "\nferma " << base << "^" << power << " mod " << primeModulo << endl;
    cout << "reduced: " << base << "^" << reducedPow << " mod " << primeModulo << endl;

    base %= primeModulo;
    int64_t res = 1;
    for (int64_t i = 0; i < reducedPow; ++i) {
        res = (res * base) % primeModulo;
    }

    return res;
}

int64_t powModBinary(int64_t base, int64_t power, int64_t modulo) {
    base %= modulo;
    int64_t res = 1;

    cout << "\nbinary " << base << "^" << power << " mod " << modulo << endl;

    while (power > 0) {
        if (power & 1) {
            cout << "power " << power << ": res = " << res << " * " << base << " mod " << modulo << " = ";
            res = (res * base) % modulo;
            cout << res << endl;
        }
        base = (base * base) % modulo;
        power >>= 1;
    }

    return res;
}

int64_t powModPrime(int64_t base, int64_t power, int64_t primeModulo) {
    power %= (primeModulo - 1);
    base %= primeModulo;
    int64_t res = 1;

    while (power > 0) {
        if (power & 1) {
            res = (res * base) % primeModulo;
        }
        base = (base * base) % primeModulo;
        power >>= 1;
    }

    return res;
}


int64_t nod(int64_t a, int64_t b) {
    if (a <= 0 || b <= 0) return -1;
    int64_t t = 0;
    while (b) {
        t = a;
        a = b;
        b = t % b;
    }
    return a;
}


void evklid
(int64_t a, int64_t b, 
vector<int64_t>& vecQ, vector<int64_t>& vecR, 
vector<int64_t>& vecX, vector<int64_t>& vecY) {
    vecQ.clear(); 
    vecR.clear();
    vecX.clear();
    vecY.clear();

    if (a <= 0 || b <= 0)
        return;

    vecR.push_back(a); vecX.push_back(1); vecY.push_back(0);
    vecR.push_back(b); vecX.push_back(0); vecY.push_back(1);
    vecQ.resize(2);

    while (vecR.back()) {
        vecQ.push_back(vecR[vecR.size() - 2] / vecR.back());
        vecX.push_back(vecX[vecX.size() - 2] - vecX.back() * vecQ.back());
        vecY.push_back(vecY[vecY.size() - 2] - vecY.back() * vecQ.back());
        vecR.push_back(vecR[vecR.size() - 2] - vecR.back() * vecQ.back());
    }
}


int64_t generateSafePrime(int64_t minVal, int64_t maxVal, mt19937& gen) {
    uniform_int_distribution<int64_t> dist((minVal - 1) / 2 + ((minVal-1) % 2), 
                                           (maxVal - 1) / 2 + ((maxVal - 1) % 2));
    while (true) {
        int64_t q = dist(gen) | 1;
        if (isPrime(q)) {
            int64_t p = 2 * q + 1;
            if (isPrime(p))
                return p;
        }
    }
}

int64_t findPrimitiveRoot(int64_t prime, mt19937& gen) {
    int64_t q = (prime - 1) / 2;
    uniform_int_distribution<int64_t> dist(2, prime - 2);
    while (true) {
        int64_t g = dist(gen);
        if (powModPrime(g, 2, prime) != 1 && powModPrime(g, q, prime) != 1)
            return g;
    }
}
