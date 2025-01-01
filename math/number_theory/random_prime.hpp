#ifndef _RANDOM_PRIME_H_
#define _RANDOM_PRIME_H_
#include <random>
#include "miller_rabin64.hpp"

// [min_n, max_n] の素数
// 範囲内に素数がないと壊れる
uint64_t random_prime(uint64_t min_n = 2, uint64_t max_n = ~0ULL){
    static std::random_device seed_gen;
    static std::mt19937_64 engine(seed_gen());
    __uint128_t len = max_n - min_n + 1;
    // https://en.wikipedia.org/wiki/Prime_number_theorem
    while (true) {
        uint64_t a = engine() % len + min_n;
        if (miller_rabin64(a)) {
            return a;
        }
    }
}
#endif