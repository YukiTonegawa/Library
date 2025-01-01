#ifndef _MONTGOMERY32_H_
#define _MONTGOMERY32_H_
#include <cstdint>
#include <cassert>

// 1 <= mod < 2^31
// modが奇数または2
struct montgomery32 {
  private:
    void _set_mod(uint32_t mod) {
        assert((mod < (1ULL << 31)) && ((mod & 1) || mod == 2));
        MOD = mod;
        NEG_INV = 0;
        uint64_t s = 1, t = 0;
        for (int i = 0; i < 32; i++) {
            if (~t & 1) {
                t += MOD;
                NEG_INV += s;
            }
            t >>= 1;
            s <<= 1;
        }
        R2 = ((uint64_t)1 << 32) % MOD;
        R2 = R2 * R2 % MOD;
        if (mod == 2) R2 = 1, NEG_INV = 1LL << 31;
        ONE = generate(1);
    }
    uint64_t MOD, NEG_INV, R2;
    uint32_t ONE;
  
  public:
    montgomery32() {}
    montgomery32(uint32_t mod) { _set_mod(mod); }
    
    void set_mod(uint32_t mod) {
        _set_mod(mod);
    }

    inline uint32_t mod() const {
        return MOD;
    }
  
    inline uint32_t one() const {
        return ONE;
    }
  
    inline uint32_t generate(uint32_t x) const {
        return reduce((uint64_t)x * R2);
    }
  
    inline uint32_t reduce(uint64_t x) const {
        x = (x + ((uint32_t)x * (uint32_t)NEG_INV) * MOD) >> 32;
        return x < MOD ? x : x - MOD;
    }

    // [0, MOD)
    inline uint32_t mul(uint32_t mx, uint32_t my) const {
        return reduce((uint64_t)mx * my);
    }

    // [0, MOD)
    inline uint32_t add(uint32_t mx, uint32_t my) const {
        uint32_t res = mx + my;
        return res >= MOD ? res - MOD : res;
    }

    // [0, MOD)
    inline uint32_t sub(uint32_t mx, uint32_t my) const {
        return mx >= my ? mx - my : mx + MOD - my;
    }

    // [0, MOD)
    inline uint32_t pow(uint32_t ma, uint32_t b) const {
        uint32_t res = one();
        while (b) {
            if (b & 1) res = mul(res, ma);
            ma = mul(ma, ma);
            b >>= 1;
        }
        return res;
    }
};
#endif