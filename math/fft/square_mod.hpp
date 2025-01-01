#ifndef _SQUARE_MOD_H_
#define _SQUARE_MOD_H_
#include "butterfly.hpp"
#include "../modint/modint32_static.hpp"

template <class mint>
std::vector<mint> square_naive(const std::vector<mint>& a) {
    int n = int(a.size());
    std::vector<mint> ans(n + n - 1);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            ans[i + j] += a[i] * a[j];
        }
    }
    return ans;
}

template <class mint>
std::vector<mint> square_fft(std::vector<mint> a) {
    int n = int(a.size());
    int z = (int)bit_ceil((unsigned int)(n + n - 1));
    a.resize(z);
    butterfly(a);
    for (int i = 0; i < z; i++) {
        a[i] *= a[i];
    }
    butterfly_inv(a);
    a.resize(n + n - 1);
    mint iz = mint(z).inv();
    for (int i = 0; i < n + n - 1; i++) a[i] *= iz;
    return a;
}

template <class mint>
std::vector<mint> square_mod(std::vector<mint>&& a) {
    int n = int(a.size());
    if (!n) return {};
    int z = (int)bit_ceil((unsigned int)(n + n - 1));
    assert((mint::mod() - 1) % z == 0);
    if (n <= 60) return square_naive(std::move(a));
    return square_fft(std::move(a));
}
template <class mint>
std::vector<mint> square_mod(const std::vector<mint>& a) {
    int n = int(a.size());
    if (!n) return {};
    int z = (int)bit_ceil((unsigned int)(n + n - 1));
    assert((mint::mod() - 1) % z == 0);
    if (n <= 60) return square_naive(a);
    return square_fft(a);
}

template <unsigned int mod = 998244353, class T>
std::vector<T> square_mod(const std::vector<T>& a) {
    int n = int(a.size());
    if (!n) return {};
    using mint = modint32_static<mod>;
    int z = (int)bit_ceil((unsigned int)(n + n - 1));
    assert((mint::mod() - 1) % z == 0);

    std::vector<mint> a2(n);
    for (int i = 0; i < n; i++) {
        a2[i] = mint(a[i]);
    }
    auto c2 = square_mod(std::move(a2));
    std::vector<T> c(n + n - 1);
    for (int i = 0; i < n + n - 1; i++) {
        c[i] = c2[i].val();
    }
    return c;
}
#endif