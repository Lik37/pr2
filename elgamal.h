#ifndef ELGAMAL_H
#define ELGAMAL_H

#include <cstdint>
#include <vector>
#include <random>

struct CipherPair {
    int64_t a, b;
};

int64_t genP(int64_t minP, int64_t maxP, std::mt19937& gen);
int64_t genG(int64_t p, std::mt19937& gen);
int64_t genX(int64_t p, std::mt19937& gen);
int64_t genY(int64_t p, int64_t g, int64_t x);
int64_t genK (int64_t p, std::mt19937& gen);
int64_t genA (int64_t p, int64_t g, int64_t k);
int64_t genB (int64_t m, int64_t p, int64_t y, int64_t k);

std::vector<int64_t> generateElgamalKeys(int64_t minP, int64_t maxP, std::mt19937& gen);


std::vector<uint8_t> pairsToBytes(const std::vector<CipherPair>& cipher);
std::vector<CipherPair> bytesToPairs(const std::vector<uint8_t>& bytes);

CipherPair elgamalEncryptByte(int64_t m, int64_t p, int64_t g, int64_t y, int64_t k);
int64_t elgamalDecryptByte(CipherPair c, int64_t p, int64_t x);

std::vector<CipherPair> elgamalEncrypt(const std::vector<uint8_t>& plaintext,
                                       int64_t p, int64_t g, int64_t y,
                                       std::mt19937& gen);
std::vector<uint8_t> elgamalDecrypt(const std::vector<CipherPair>& ciphertext,
                                    int64_t p, int64_t x);

#endif