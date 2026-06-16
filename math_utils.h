#ifndef MATH_UTILS_H
#define  MATH_UTILS_H

#include <cstdint>
#include <vector>
#include <random>

bool isPrime(int64_t n);

int64_t powModFerma(int64_t base, int64_t pow, int64_t primeModulo);
int64_t powModBinary(int64_t base, int64_t pow, int64_t modulo);
int64_t powModPrime(int64_t base, int64_t power, int64_t primeModulo);

int64_t nod(int64_t a, int64_t b);

void evklid
(int64_t a, int64_t b, 
std::vector<int64_t>& vecQ, std::vector<int64_t>& vecR, 
std::vector<int64_t>& vecX, std::vector<int64_t>& vecY);

int64_t generateSafePrime(int64_t minVal, int64_t maxVal, std::mt19937& gen);
int64_t findPrimitiveRoot(int64_t p, std::mt19937& gen);

#endif