#ifndef _BIGFLOAT_H_
#define _BIGFLOAT_H_
/*
#include "base_bigint.hpp"

template<int precision>
struct bigfloat : std::vector<base_bigint::i64> {
 private:
    using i64 = base_bigint::i64;
    using i128 = base_bigint::i128;
    using std::vector<i64>::vector;
    static constexpr i64 base = base_bigint::base;
    static constexpr int base_log = base_bigint::base_log;
    bool s;
    int exp10;

    // -1 : (|l| < |r|), 0 : (|l| == |r|), 1 : (|l| > |r|)
    static int cmp_abs(const bigfloat &l, const bigfloat &r) {
        int n = l.size(), m = r.size();
        for (int i = std::max(n, m) - 1; i >= 0; i--) {
            i64 x = i >= n ? 0 : l[i];
            i64 y = i >= m ? 0 : r[i];
            if (x < y) return -1;
            if (x > y) return 1;
        }
        return 0;
    }
    // -1 : (l < r), 0 : (l == r), 1 : (l > r)
    static int cmp(const bigfloat &l, const bigfloat &r) {
        if (l.s != r.s) return l.s ? -1 : 1;
        int res = cmp_abs(l, r);
        if (l.s) res *= -1;
        return res;
    }
    bigfloat prefix(int d) const {
        int deg = base_bigint::deg(*this);
        int n = std::min(deg, d);
        auto res = bigfloat(this->s, this->exp10 + deg - n, base_bigint::rshift(*this, deg - n));
        while (!res.empty() && res.back() == 0) res.pop_back();
        return res; 
    }
    bigfloat inv_newton(int k) const {
        int d = base_bigint::deg(*this);
        assert(d != -1);
        bigfloat x((i64)1);
        x.exp10 = -(d + 1);
        bigfloat pre = prefix(base_log);
        for (int i = 0; i < 5; i++) {
            bigfloat y = pre * x.square();
            x = (x + x - y);
            x = x.prefix(base_log);
        }
        x = x.prefix(base_log);
        for (int i = 1; i < k; i *= 2) {
            bigfloat y = prefix(std::max(base_log, i << 1)) * x.square();
            x = (x + x - y);
            x = x.prefix(std::max(base_log, i << 1));
        }
        return x.prefix(k);
    }

    bigfloat div1(const bigfloat &r) {
        if (r > (*this)) return (*this) = bigfloat();
        int d = base_bigint::deg(*this);
        if(d < base_log) return (*this)[0] / r[0];
        return (*this) *= r.inv_newton(d + 1);
    }

  public:
    bigfloat(int _s, int _exp10, const std::vector<i64> &_v) : std::vector<i64>(_v), s(_s), exp10(_exp10) {}
    bigfloat(std::string &S) : std::vector<i64>(base_bigint::to_vector(S)), s(!S.empty() && S[0] == '-'), exp10(0) {}
    bigfloat(i64 x) : s(0), exp10(0) {
        if (x < 0) s = 1, x *= -1;
        this->push_back(x);
    } 
    bigfloat(int x) : bigfloat((i64)x) {}
    bigfloat() : s(0), exp10(0) {}
    

    std::string to_string() const {
        assert(exp10 >= 0);
        std::string A = "";
        if (s) A += '-';
        std::string B = base_bigint::to_string(*this);
        if (B[0] == '0') return B;
        return A + B + std::string(exp10, '0');
    }

    // return {this / vr, this % vr}
    std::pair<bigfloat, bigfloat> div(const bigfloat &vr) const {
        assert(!this->s && !vr.s);
        bigfloat tmp = bigfloat(*this).div1(vr);
        bigfloat q(tmp.s, 0, base_bigint::rshift(tmp, -tmp.exp10));
        bigfloat r = *this - (vr * q);
        if (vr <= r) {
            r -= vr;
            q = q + 1LL;
        }
        return {q, r};
    }

    bigfloat square() const { return bigfloat(0, exp10 * 2, base_bigint::mul_bigint::square(*this)); }
    bigfloat operator += (const bigfloat &r) { return *this = *this + r; }
    bigfloat operator -= (const bigfloat &r) { return *this = *this - r; }
    bigfloat operator *= (const bigfloat &r) {
        if (this->empty() || r.empty()) return *this = bigfloat();
        return (*this) = bigfloat(s ^ r.s, exp10 + r.exp10, base_bigint::mul_bigint::mul(*this, r));
    }
    bigfloat operator /= (const bigfloat &r) { return *this = this->div(r).first; }
    bigfloat operator %= (const bigfloat &r) { return *this = this->div(r).second; }
    bigfloat operator + (const bigfloat &r) const {
        if (this->s != r.s) {
            bigfloat tmp = r;
            tmp.s = !tmp.s;
            return *this - tmp;
        }
        int expmin = std::min(this->exp10, r.exp10);
        return bigfloat(this->s, expmin, base_bigint::add(base_bigint::lshift(*this, this->exp10 - expmin), base_bigint::lshift(r, r.exp10 - expmin)));
    }

    bigfloat operator - (const bigfloat &r) const {
        if (this->s != r.s) {
            bigfloat tmp = r;
            tmp.s = !tmp.s;
            return *this + tmp;
        }
        int expmin = std::min(this->exp10, r.exp10);
        bool s = this->s;
        auto v = base_bigint::sub(base_bigint::lshift(*this, this->exp10 - expmin), base_bigint::lshift(r, r.exp10 - expmin));
        if (!v.empty() && v.back() == -1) {
            s = !s;
            bool down = 0;
            for (int i = 0; i < int(v.size()); i++) {
                v[i] = -(v[i] + down);
                if ((down = (v[i] < 0))) v[i] += base;
            }
        }
        return bigfloat(s, expmin, v);
    }
    bigfloat operator * (const bigfloat &r) const { return bigfloat(*this) *= r; }
    bigfloat operator / (const bigfloat &r) const { return this->div(r).first; }
    bigfloat operator % (const bigfloat &r) const { return this->div(r).second; }
    bigfloat operator - () const { bigfloat res(*this); res.s = !res.s; return res; }
    bool operator == (const bigfloat &r) const { return cmp(*this, r) == 0; }
    bool operator < (const bigfloat &r) const { return cmp(*this, r) == -1; }
    bool operator > (const bigfloat &r) const { return cmp(*this, r) == 1; }
    bool operator >= (const bigfloat &r) const { return cmp(*this, r) >= 0; }
    bool operator <= (const bigfloat &r) const { return cmp(*this, r) <= 0; }
    // 0の場合は未定義, 正なら0, 負なら1
    bool sign() const { return s; }
};
*/
#endif