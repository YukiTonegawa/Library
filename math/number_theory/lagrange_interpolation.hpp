#ifndef _LAGRANGE_H_
#define _LAGRANGE_H_
#include "../modint/combination.hpp"

// n次以下の多項式に対し
// f(0) ~ f(n)を与えf(t)を求める
// O(n log(MOD))
template<typename mint>
mint lagrange(const std::vector<mint> &y, mint t) {
    using cmb = combination_mod<mint>;
    int N = y.size();
    if (t.val() < N) return y[t.val()];
    cmb::build(N);
    mint M = 1, res = 0;    
    for (int i = 0; i < N; i++) {
        M *= t - i;
        mint iQ = cmb::ifac(i) * cmb::ifac(N - 1 - i);
        if ((N - 1 - i) & 1) iQ *= -1;
        res += y[i] * iQ * cmb::inv_large(t.val() - i);
    }
    return res * M;
}

// n次以下の多項式に対し
// f(x0), f(x0 + d), f(x0 + 2d)...f(x0 + nd)からf(t)を求める
template<typename mint>
mint lagrange_arithmetic(mint x0, mint d, const std::vector<mint> &y, mint t) {
    assert(d.val() != 0);
    // x0 + dk = t
    // -> k = (t - x0) / d
    mint k = (t - x0) / d;
    return lagrange<mint>(y, k);
}

// sum(r^{i} * i^{d}) 0 <= i < n
template<typename mint>
mint riid(mint r, int d, long long n) {
    using cmb = combination_mod<mint>;
    if (n == 0) return 0;
    if (r.val() == 0) {
        return d == 0 ? mint(1) : 0;
    }
    n--;
    std::vector<mint> y(d + 2);
    
    {
        std::vector<bool> is_prime(d + 2, 1);
        y[0] = mint(0).pow(d);
        y[1] = mint(1).pow(d);
        for (int i = 2; i < d + 2; i++) {
            if (!is_prime[i]) continue;
            y[i] = mint(i).pow(d);
            for (int j = 2 * i, k = 2; j < d + 2; j += i, k++) {
                is_prime[j] = false;
                y[j] = y[i] * y[k];
            }
        }
    }

    mint tmp = 0, rpow = 1, last = r.pow(n % (mint::mod() - 1));
    n %= mint::mod();
    for (int i = 0; i < d + 2; i++) {
        tmp += rpow * y[i];
        rpow *= r;
        y[i] = tmp;
    }

    if (r.val() == 1) return lagrange<mint>(y, n);
    cmb::build(d + 1);
    
    mint minus_r_pow = mint(-r).pow(d);
    mint minus_r_inv = mint(-r).inv();
    mint comb = 1, c = 0;
    for (int i = 0; i < d + 1; i++, minus_r_pow *= minus_r_inv) {
        comb *= cmb::inv(i + 1) * mint(d + 1 - i);
        c += comb * minus_r_pow * y[i];
    }
    c *= mint(1 - r).pow(d + 1).inv();
    
    mint powerRinv = 1, rinv = -minus_r_inv;
    for (int i = 0; i < d + 1; i++, powerRinv *= rinv) y[i] = (y[i] - c) * powerRinv;
    
    y.pop_back();
    return c + last * lagrange<mint>(y, n);
}
#endif