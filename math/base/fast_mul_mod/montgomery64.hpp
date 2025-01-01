#ifndef _MONTGOMERY64_H_
#define _MONTGOMERY64_H_
#include <cstdint>
#include <cassert>

// 0 < MOD < 2^63
// modが奇数または2
struct montgomery64{
  private:
    void _set_mod(uint64_t mod) {
        assert((mod < (1ULL << 63)) && ((mod & 1) || mod == 2));
        MOD = mod;
        NEG_INV = 0;
        __uint128_t s = 1, t = 0;
        for (int i = 0; i < 64; i++) {
            if (~t & 1) {
                t += MOD;
                NEG_INV += s;
            }
            t >>= 1;
            s <<= 1;
        }
        R2 = ((__uint128_t)1 << 64) % MOD;
        R2 = R2 * R2 % MOD;
        if (mod == 2) R2 = 1, NEG_INV = 1ULL << 63;
        ONE = generate(1);
    }
    __uint128_t MOD, NEG_INV, R2;
    uint64_t ONE;

  public:
    montgomery64() {}
    montgomery64(uint64_t mod) { _set_mod(mod); }
  
    void set_mod(uint64_t mod) {
        _set_mod(mod);
    }
  
    inline uint64_t mod() const {
        return MOD;
    }
  
    inline uint64_t one() const {
        return ONE;
    }

    inline uint64_t generate(uint64_t x) const {
        return reduce((__uint128_t)x * R2);
    }
  
    inline uint64_t reduce(__uint128_t x) const {
        x = (x + ((uint64_t)x * (uint64_t)NEG_INV) * MOD) >> 64;
        return x < MOD ? x : x - MOD;
    }

    // [0, MOD)
    inline uint64_t mul(uint64_t mx, uint64_t my) const {
        return reduce((__uint128_t)mx * my);
    }

    // [0, MOD)
    inline uint64_t add(uint64_t mx, uint64_t my) const {
        uint64_t res = mx + my;
        return res >= MOD ? res - MOD : res;
    }

    // [0, MOD)
    inline uint64_t sub(uint64_t mx, uint64_t my) const {
        return mx >= my ? mx - my : mx + MOD - my;
    }

    // [0, MOD)
    inline uint64_t pow(uint64_t ma, uint64_t b) const {
        uint64_t res = one();
        while (b) {
            if (b & 1) res = mul(res, ma);
            ma = mul(ma, ma);
            b >>= 1;
        }
        return res;
    }
};
#endif