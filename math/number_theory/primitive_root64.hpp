#ifndef _PRIMITIVE_ROOT64_H_
#define _PRIMITIVE_ROOT64_H_

#include <random> 
#include "rho.hpp"

// p: 素数
uint64_t primitive_root64(uint64_t p) {
    static std::random_device seed_gen;
    static std::mt19937_64 engine(seed_gen());
    assert(miller_rabin64(p));
    auto primes = rho::prime_factor(p - 1);
    montgomery64 mr(p);
    while (true) {
        bool f = true;
        uint64_t a = engine() % (p - 1) + 1;
        uint64_t A = mr.generate(a);
        for (uint64_t pk : primes){
            // a ^ (p - 1) / pk ≡ 1 (mod p) -> no
            if (mr.pow(A, (p - 1) / pk) == mr.one()) {
                f = false;
                break;
            }
        }
        if (f) return a;
    }
}

#endif