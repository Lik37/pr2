#include <iostream>
#include <cstdint>
#include <vector>
#include <random>

#include "elgamal.h"
#include "math_utils.h"

using namespace std;


int64_t genP(int64_t minP, int64_t maxP, mt19937& gen) {
    return generateSafePrime(minP, maxP, gen);
}
int64_t genG(int64_t p, mt19937& gen) {
    return findPrimitiveRoot(p, gen);
}
int64_t genX(int64_t p, mt19937& gen) {
    return uniform_int_distribution<int64_t>(2, p - 2)(gen);
}
int64_t genY(int64_t p, int64_t g, int64_t x) {
    return powModPrime(g, x, p);
}
int64_t genK (int64_t p, mt19937& gen) { 
    uniform_int_distribution<int64_t> dist(2, p - 2);
    int64_t k;
    do {
        k = dist(gen);
    } while (nod(k, p - 1) != 1);
    return k;
}
int64_t genA (int64_t p, int64_t g, int64_t k) { 
    return powModPrime(g, k, p);
}
int64_t genB (int64_t m, int64_t p, int64_t y, int64_t k) { 
    return (powModPrime(y, k, p) * m) % p;
}

vector<int64_t> generateElgamalKeys(int64_t minP, int64_t maxP, mt19937& gen) {
    vector<int64_t> keys;
    keys.push_back(genP(minP, maxP, gen)); // p
    keys.push_back(genG(keys[0], gen)); // g
    keys.push_back(genX(keys[0], gen)); // x
    keys.push_back(genY(keys[0], keys[1], keys[2])); // y
    return keys;
}


vector<uint8_t> pairsToBytes(const vector<CipherPair>& cipher) {
    if (cipher.empty()) return {};
    const uint8_t* bytePointer = reinterpret_cast<const uint8_t*>(cipher.data());
    size_t totalBytes = cipher.size() * sizeof(CipherPair);
    return vector<uint8_t>(bytePointer, bytePointer + totalBytes);
}

vector<CipherPair> bytesToPairs(const vector<uint8_t>& bytes) {
    if (bytes.empty() || (bytes.size() % sizeof(CipherPair) != 0)) return {};
    const CipherPair* cipherPointer = reinterpret_cast<const CipherPair*>(bytes.data());
    size_t qntPairs = bytes.size() / sizeof(CipherPair);
    return vector<CipherPair>(cipherPointer, cipherPointer + qntPairs);
}


CipherPair elgamalEncryptByte(int64_t m, int64_t p, int64_t g, int64_t y, int64_t k) {
    int64_t a = genA(p, g, k);
    int64_t b = genB(m, p, y, k);
    return {a, b};
}

int64_t elgamalDecryptByte(CipherPair c, int64_t p, int64_t x) {
    int64_t aInv = powModPrime(c.a, (p - 1) - x, p); // +(p-1) по теореме ферма
    return (c.b * aInv) % p;
}


vector<CipherPair> elgamalEncrypt(const vector<uint8_t>& plaintext,
                                  int64_t p, int64_t g, int64_t y,
                                  mt19937& gen) {
    vector<CipherPair> result;
    if (plaintext.empty()) return result;

    for (uint8_t m : plaintext) {
        int64_t k = genK(p, gen);

        CipherPair cipher = elgamalEncryptByte(m, p, g, y, k);
        cout << "m=" << m << " -> (k=" << k << ") a=" << cipher.a << " b=" << cipher.b << "\n";
        result.push_back(cipher);
    }
    return result;
}

vector<uint8_t> elgamalDecrypt(const vector<CipherPair>& ciphertext,
                               int64_t p, int64_t x) {
    vector<uint8_t> result;
    for (const auto& c : ciphertext) {
        int64_t m = elgamalDecryptByte(c, p, x);
        cout << "a=" << c.a << " b=" << c.b << " -> m=" << m << "\n";
        result.push_back(static_cast<uint8_t>(m));
    }
    return result;
}
