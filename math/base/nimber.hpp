#ifndef _NIMBER_H_
#define _NIMBER_H_
#include <array>
#include <numeric>

struct nimber {
    using self_t = nimber;

    static uint64_t nim_product(uint64_t a, uint64_t b) {
        static constexpr uint64_t th[6] = {(1 << 1), (1 << 2), (1 << 4), (1 << 8), (1 << 16), (1ULL << 32)};
        static constexpr uint64_t small = th[3];
        static constexpr uint32_t inf = ~0;
        static std::array<std::array<uint32_t, small>, small> prod_small;
        static bool first = true;
        if (first) {
            first = false;
            for (int i = 0; i < small; i++) {
                std::fill(prod_small[i].begin(), prod_small[i].end(), inf);
            }
            prod_small[0][0] = prod_small[0][1] = prod_small[1][0] = 0;
            prod_small[1][1] = 1;
        }
        if (a < b) std::swap(a, b);
        if (a < small && prod_small[a][b] != inf) return prod_small[a][b];
        for (int i = 5; i >= 0; i--) {
            if (a >= th[i]) {
                uint64_t au = a / th[i], al = a % th[i];
                uint64_t bu = b / th[i], bl = b % th[i];
                uint64_t aubu = nim_product(au, bu);
                uint64_t x = (aubu ^ nim_product(au, bl) ^ nim_product(al, bu)) * th[i];
                uint64_t y = nim_product(aubu, th[i] / 2);
                uint64_t z = nim_product(al, bl);
                if (a < small) return prod_small[a][b] = prod_small[b][a] = x ^ y ^ z;
                return x ^ y ^ z;
            }
        }
        return prod_small[a][b];
    }

    uint64_t x;

    nimber(uint64_t _x) : x(_x) {}

    self_t operator += (const nimber &b) { x ^= b.x; return *this; }
    self_t operator *= (const nimber &b) { x = nim_product(x, b.x); return *this; }
    self_t operator + (const nimber &b) const { self_t res(*this); return res += b; }
    self_t operator * (const nimber &b) const { self_t res(*this); return res *= b; }
    uint64_t val() const { return x; }
};
#endif