#ifndef _F128_H_
#define _F128_H_

/*
struct f128 {
  private:
    using u128 = __uint128_t;
    static constexpr int begin_sign = 127, end_sign = 128;
    static constexpr int begin_frac = 10, end_frac = 127;
    static constexpr int begin_exp = 0, end_exp = 10;
    static constexpr u128 mask_sign = u128(1) << begin_sign;
    static constexpr u128 mask_frac = (u128(1) << end_frac) - (u128(1) << begin_frac);
    static constexpr u128 mask_exp = (u128(1) << end_exp) - (u128(1) << begin_exp);
    static constexpr u128 inf_frac = mask_frac + 1; // 仮数[0, inf_frac)
    static constexpr int inf_exp = 1 << (end_exp - 1); // 指数[-inf_exp, inf_exp)
    u128 x;
    int extract_sign() const { return x >> begin_sign; }
    u128 extract_frac() const { return (x & mask_frac) >> begin_frac; }
    u128 extract_frac_one() const { return ((x & mask_frac) + mask_sign) >> begin_frac; } // 省略された1.0を足した値
    int extract_exp() const { return int(x & mask_exp) - inf_exp; }
  
  public:
    
    f128(long long a) {
        x = (a < 0 ? abs(a) : a);
        //int msb = __builtin_
        if (a < 0) x |= mask_sign;
    }
    f128(unsigned long long a) {
        //
    }
    f128(int a) : f128((long long)a) {}
    f128(unsigned a) : f128((unsigned long long)a) {}
    f128(double a) {
        //
    }
    f128(float a) : f128((double)a) {}

    f128(std::string S) {
        //
    }
    
    f128 operator - () const {
        f128 res(*this);
        res.x ^= mask_sign;
        return res;
    } 

    f128 operator += (const f128 &r) {
        if ((x ^ r.x) >> begin_sign) return *this -= -r;
        int e1 = extract_exp(), e2 = r.extract_exp(), eM = std::max(e1, e2);
        u128 f1 = extract_frac_one(), f2 = r.extract_frac_one();
        f1 >>= (eM - e1), f2 >>= (eM - e2);
        f1 += f2;
        while (f1 >= 2 * inf_frac) {
            f1 >>= 1;
            eM++;
        }
        assert(eM < inf_exp);
        x &= mask_sign;
        x |= (f1 << begin_frac) & mask_frac;
        x |= eM + inf_exp;
        return *this;
    }

    f128 operator -= (const f128 &r) {
        if ((x ^ r.x) >> begin_sign) return *this += -r;
        int e1 = extract_exp(), e2 = r.extract_exp(), eM = std::max(e1, e2);
        u128 f1 = extract_frac_one(), f2 = r.extract_frac_one();
        f1 >>= (eM - e1), f2 >>= (eM - e2);
        if (f1 < f2) {
            x ^= mask_sign;
            std::swap(f1, f2);
        }
        f1 -= f2;
        while (f1 < inf_frac) {
            f1 <<= 1;
            eM--;
        }
        assert(eM >= -inf_exp);
        x &= mask_sign;
        x |= (f1 << begin_frac) & mask_frac;
        x |= eM + inf_exp;
        return *this;
    }

    f128 operator *= (const f128 &r) {
        static constexpr int len_frac = end_frac - begin_frac + 1;
        static constexpr int len_half_frac = len_frac / 2;
        static constexpr u128 mask_lower = (u128(1) << len_half_frac) - 1;
        x ^= r.x & mask_sign;
        int e1 = extract_exp(), e2 = r.extract_exp();
        u128 f1 = extract_frac_one(), f2 = r.extract_frac_one();
        u128 fa = f1 >> len_half_frac, fb = f1 & mask_lower;
        u128 fc = f2 >> len_half_frac, fd = f2 & mask_lower;
        fb = (fb * fc) >> len_half_frac;
        fd = (fa * fd) >> len_half_frac;
        fa = (fa * fc) + fb + fd;
        e1 += e2;
        while (fa >= 2 * inf_frac) {
            fa >>= 1;
            e1++;
        }
        assert(-inf_exp <= e1 && e1 < inf_exp);
        x &= mask_sign;
        x |= (fa << begin_frac) & mask_frac;
        x |= e1 + inf_exp;
        return *this;
    }

    f128 inv() const {
        static constexpr int len_frac = end_frac - begin_frac + 1;
        static constexpr int shift = 127;
        static constexpr u128 a = u128(1) << shift;
        u128 f1 = extract_frac_one();
        u128 f2 = f1 >> (len_frac - 63);
        f2 = a / f2;
        //
    }

    f128 operator /= (const f128 &r) {
        static constexpr int len_frac = end_frac - begin_frac + 1;
        static constexpr int len_half_frac = len_frac / 2;
        static constexpr u128 mask_lower = (u128(1) << len_half_frac) - 1;
        //
    }

    f128 operator + (const f128 &r) const { return f128(*this) += r; }
    f128 operator - (const f128 &r) const { return f128(*this) -= r; }
    f128 operator * (const f128 &r) const { return f128(*this) *= r; }
    f128 operator / (const f128 &r) const { return f128(*this) /= r; }
    
    std::string to_string() const {
        //
    }

    f128 sqrt() const {
        //
    }
    
    f128 log() const {
        //
    }

    f128 exp() const {
        //
    }
};
*/
#endif