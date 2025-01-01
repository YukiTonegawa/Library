#ifndef _BIGBINARY_H_
#define _BIGBINARY_H_
#include "base_bigbinary.hpp"

struct bigbinary : std::vector<base_bigbinary::i64> {
 private:
    using i64 = base_bigbinary::i64;
    using i128 = base_bigbinary::i128;
    using std::vector<i64>::vector;
    static constexpr i64 base = base_bigbinary::base;
    static constexpr int base_log = base_bigbinary::base_log;
    bool s;
    int exp2;

    // -1 : (|l| < |r|), 0 : (|l| == |r|), 1 : (|l| > |r|)
    static int cmp_abs(const bigbinary &l, const bigbinary &r) {
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
    static int cmp(const bigbinary &l, const bigbinary &r) {
        if (l.s != r.s) return l.s ? -1 : 1;
        int res = cmp_abs(l, r);
        if (l.s) res *= -1;
        return res;
    }
    bigbinary prefix(int d) const {
        int deg = base_bigbinary::deg(*this);
        int n = std::min(deg, d);
        auto res = bigbinary(this->s, this->exp2 + deg - n, base_bigbinary::rshift(*this, deg - n));
        while (!res.empty() && res.back() == 0) res.pop_back();
        return res; 
    }
    bigbinary inv_newton(int k) const {
        int d = base_bigbinary::deg(*this);
        assert(d != -1);
        bigbinary x((i64)1);
        x.exp2 = -(d + 1);
        bigbinary pre = prefix(base_log);
        for (int i = 0; i < 5; i++) {
            bigbinary y = pre * x.square();
            x = (x + x - y);
            x = x.prefix(base_log);
        }
        x = x.prefix(base_log);
        for (int i = 1; i < k; i *= 2) {
            bigbinary y = prefix(std::max(base_log, i << 1)) * x.square();
            x = (x + x - y);
            x = x.prefix(std::max(base_log, i << 1));
        }
        return x.prefix(k);
    }

    bigbinary div1(const bigbinary &r) {
        if (r > (*this)) return (*this) = bigbinary();
        int d = base_bigbinary::deg(*this);
        if(d < base_log) return (*this)[0] / r[0];
        return (*this) *= r.inv_newton(d + 1);
    }

  public:
    bigbinary(int _s, int _exp2, const std::vector<i64> &_v) : std::vector<i64>(_v), s(_s), exp2(_exp2) {}
    bigbinary(std::string &S) : std::vector<i64>(base_bigbinary::to_vector(S)), s(!S.empty() && S[0] == '-'), exp2(0) {}
    bigbinary(i64 x) : s(0), exp2(0) {
        if (x < 0) s = 1, x *= -1;
        this->push_back(x);
    } 
    bigbinary(int x) : bigbinary((i64)x) {}
    bigbinary() : s(0), exp2(0) {}
    
    std::string to_string() const {
        assert(exp2 >= 0);
        std::string A = "";
        if (s) A += '-';
        std::string B = base_bigbinary::to_string(*this);
        if (B[0] == '0') return B;
        return A + B + std::string(exp2, '0');
    }

    // return {this / vr, this % vr}
    std::pair<bigbinary, bigbinary> div(const bigbinary &vr) const {
        assert(!this->s && !vr.s);
        bigbinary tmp = bigbinary(*this).div1(vr);
        bigbinary q(tmp.s, 0, base_bigbinary::rshift(tmp, -tmp.exp2));
        bigbinary r = *this - (vr * q);
        if (vr <= r) {
            r -= vr;
            q = q + 1LL;
        }
        return {q, r};
    }

    bigbinary square() const { return bigbinary(0, exp2 * 2, base_bigbinary::mul_bigbinary::square(*this)); }
    bigbinary operator += (const bigbinary &r) { return *this = *this + r; }
    bigbinary operator -= (const bigbinary &r) { return *this = *this - r; }
    bigbinary operator *= (const bigbinary &r) {
        if (this->empty() || r.empty()) return *this = bigbinary();
        return (*this) = bigbinary(s ^ r.s, exp2 + r.exp2, base_bigbinary::mul_bigbinary::mul(*this, r));
    }
    bigbinary operator /= (const bigbinary &r) { return *this = this->div(r).first; }
    bigbinary operator %= (const bigbinary &r) { return *this = this->div(r).second; }
    bigbinary operator + (const bigbinary &r) const {
        if (this->s != r.s) {
            bigbinary tmp = r;
            tmp.s = !tmp.s;
            return *this - tmp;
        }
        int expmin = std::min(this->exp2, r.exp2);
        return bigbinary(this->s, expmin, base_bigbinary::add(base_bigbinary::lshift(*this, this->exp2 - expmin), base_bigbinary::lshift(r, r.exp2 - expmin)));
    }

    bigbinary operator - (const bigbinary &r) const {
        if (this->s != r.s) {
            bigbinary tmp = r;
            tmp.s = !tmp.s;
            return *this + tmp;
        }
        int expmin = std::min(this->exp2, r.exp2);
        bool s = this->s;
        auto v = base_bigbinary::sub(base_bigbinary::lshift(*this, this->exp2 - expmin), base_bigbinary::lshift(r, r.exp2 - expmin));
        if (!v.empty() && v.back() == -1) {
            s = !s;
            bool down = 0;
            for (int i = 0; i < int(v.size()); i++) {
                v[i] = -(v[i] + down);
                if ((down = (v[i] < 0))) v[i] += base;
            }
        }
        return bigbinary(s, expmin, v);
    }
    bigbinary operator * (const bigbinary &r) const { return bigbinary(*this) *= r; }
    bigbinary operator / (const bigbinary &r) const { return this->div(r).first; }
    bigbinary operator % (const bigbinary &r) const { return this->div(r).second; }
    bigbinary operator - () const { bigbinary res(*this); res.s = !res.s; return res; }
    bool operator == (const bigbinary &r) const { return cmp(*this, r) == 0; }
    bool operator < (const bigbinary &r) const { return cmp(*this, r) == -1; }
    bool operator > (const bigbinary &r) const { return cmp(*this, r) == 1; }
    bool operator >= (const bigbinary &r) const { return cmp(*this, r) >= 0; }
    bool operator <= (const bigbinary &r) const { return cmp(*this, r) <= 0; }
    // 0の場合は未定義, 正なら0, 負なら1
    bool sign() const { return s; }
};


bigbinary hex_to_bigbinary(const std::string &S) {
    assert(!S.empty());
    std::string s = "";
    int pos = 0;
    if (S[pos] == '-') s += '-', pos++;
    for ( ; pos < int(S.size()); pos++) {
        char c = S[pos];
        int x = 0;
        if ('0' <= c && c <= '9') x = c - '0';
        else x = c - 'A' + 10;
        for (int j = 3; j >= 0; j--) {
            bool f = (x >> j) & 1;
            if ((s.empty() || s.back() == '-') && !f) continue;
            s += (f ? '1' : '0');
        }
    }
    return bigbinary(s);
}

std::string to_hex(const bigbinary &a) {
    std::string s = "";
    if (a.sign()) s += '-';
    for (int i = int(a.size()) - 1; i >= 0; i--) {
        for (int j = 14; j >= 0; j--) {
            int x = (a[i] >> (4 * j)) & 15;
            if ((s.empty() || s.back() == '-') && !x) continue;
            s += (x < 10 ? x + '0' : (x - 10) + 'A');
        }
    }
    if (s.empty() || (s.size() == 1 && s[0] == '-')) s = "0";
    return s;
}
#endif
