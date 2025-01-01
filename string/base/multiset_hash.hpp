#ifndef _SET_HASH_H_
#define _SET_HASH_H_
#include "../../math/base/gcd.hpp"
#include "../../math/base/fast_mul_mod/montgomery64.hpp"
#include "../../math/number_theory/random_prime.hpp"

struct multiset_hash {
  private:
    static uint64_t M, x;
    static montgomery64 mr;
    uint64_t _hash;
  
  public:
    
    multiset_hash() : _hash(1) {}

    // (A0 + x) * (A1 + x) ...
    multiset_hash(std::vector<long long> &V) {
        _hash = mr.one();
        for (long long v : V) {
            _hash = mr.mul(_hash, mr.generate(uint64_t(v) + x));
        }
    }

    uint64_t hash() {
        return _hash;
    }
    
    void insert(long long v) {
        _hash = mr.mul(_hash, mr.generate(uint64_t(v) + x));
    }

    // vが無い状態でeraseすると壊れる
    void erase(long long v) {
        uint64_t d = uint64_t(v) + x;
        auto [g, inv] = inv_gcd(d, M);
        assert(g == 1);
        _hash = mr.mul(_hash, mr.generate(inv));
    }

    static multiset_hash merge(multiset_hash a, multiset_hash b) {
        multiset_hash res;
        res._hash = mr.mul(a._hash, b._hash);
        return res;
    }
};

// 0 <= Ai < 2^62なら Ai + x < M
uint64_t multiset_hash::M = random_prime((1ULL << 63) - (1LL << 50), (1ULL << 63) - 1);
uint64_t multiset_hash::x = random_prime((1ULL << 60), (1ULL << 62) - (1ULL << 50));
montgomery64 multiset_hash::mr = montgomery64(multiset_hash::M);

#endif