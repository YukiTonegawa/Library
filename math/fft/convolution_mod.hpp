#ifndef _CONVOLUTION_MOD_H_
#define _CONVOLUTION_MOD_H_
#include "butterfly.hpp"
#include "../modint/modint32_static.hpp"

template <class mint>
std::vector<mint> convolution_fft(std::vector<mint> a, std::vector<mint> b) {
    int n = int(a.size()), m = int(b.size());
    int z = (int)bit_ceil((unsigned int)(n + m - 1));
    a.resize(z);
    butterfly(a);
    b.resize(z);
    butterfly(b);
    for (int i = 0; i < z; i++) {
        a[i] *= b[i];
    }
    butterfly_inv(a);
    a.resize(n + m - 1);
    mint iz = mint(z).inv();
    for (int i = 0; i < n + m - 1; i++) a[i] *= iz;
    return a;
}

template <class mint>
std::vector<mint> convolution_mod(std::vector<mint>&& a, std::vector<mint>&& b) {
    int n = int(a.size()), m = int(b.size());
    if (!n || !m) return {};
    int z = (int)bit_ceil((unsigned int)(n + m - 1));
    assert((mint::mod() - 1) % z == 0);
    if (std::min(n, m) <= 60) return convolution_naive(std::move(a), std::move(b));
    return convolution_fft(std::move(a), std::move(b));
}
template <class mint>
std::vector<mint> convolution_mod(const std::vector<mint>& a, const std::vector<mint>& b) {
    int n = int(a.size()), m = int(b.size());
    if (!n || !m) return {};
    int z = (int)bit_ceil((unsigned int)(n + m - 1));
    assert((mint::mod() - 1) % z == 0);
    if (std::min(n, m) <= 60) return convolution_naive(a, b);
    return convolution_fft(a, b);
}

template <unsigned int mod = 998244353, class T>
std::vector<T> convolution_mod(const std::vector<T>& a, const std::vector<T>& b) {
    int n = int(a.size()), m = int(b.size());
    if (!n || !m) return {};
    using mint = modint32_static<mod>;
    int z = (int)bit_ceil((unsigned int)(n + m - 1));
    assert((mint::mod() - 1) % z == 0);

    std::vector<mint> a2(n), b2(m);
    for (int i = 0; i < n; i++) {
        a2[i] = mint(a[i]);
    }
    for (int i = 0; i < m; i++) {
        b2[i] = mint(b[i]);
    }
    auto c2 = convolution_mod(std::move(a2), std::move(b2));
    std::vector<T> c(n + m - 1);
    for (int i = 0; i < n + m - 1; i++) {
        c[i] = c2[i].val();
    }
    return c;
}
#endif