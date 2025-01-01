#ifndef _BUTTERFLY64_H_
#define _BUTTERFLY64_H_
#include <algorithm>
#include <array>
#include <cassert>
#include <type_traits>
#include <vector>
#include "../number_theory/primitive_root64.hpp"
#include "../base/bit_ceil.hpp"

template <class mint>
struct fft64_info {
    static constexpr int rank2 = __builtin_ctzll(mint::mod() - 1);
    static unsigned long long g;

    std::array<mint, rank2 + 1> root;   // root[i]^(2^i) == 1
    std::array<mint, rank2 + 1> iroot;  // root[i] * iroot[i] == 1

    std::array<mint, std::max(0, rank2 - 2 + 1)> rate2;
    std::array<mint, std::max(0, rank2 - 2 + 1)> irate2;

    std::array<mint, std::max(0, rank2 - 3 + 1)> rate3;
    std::array<mint, std::max(0, rank2 - 3 + 1)> irate3;

    fft64_info() {
        if (g == 0) g = primitive_root64(mint::mod());
        root[rank2] = mint(g).pow((mint::mod() - 1) >> rank2);
        iroot[rank2] = root[rank2].inv();
        for (int i = rank2 - 1; i >= 0; i--) {
            root[i] = root[i + 1] * root[i + 1];
            iroot[i] = iroot[i + 1] * iroot[i + 1];
        }

        {
            mint prod = 1, iprod = 1;
            for (int i = 0; i <= rank2 - 2; i++) {
                rate2[i] = root[i + 2] * prod;
                irate2[i] = iroot[i + 2] * iprod;
                prod *= iroot[i + 2];
                iprod *= root[i + 2];
            }
        }
        {
            mint prod = 1, iprod = 1;
            for (int i = 0; i <= rank2 - 3; i++) {
                rate3[i] = root[i + 3] * prod;
                irate3[i] = iroot[i + 3] * iprod;
                prod *= iroot[i + 3];
                iprod *= root[i + 3];
            }
        }
    }
};
template<typename mint>
unsigned long long fft64_info<mint>::g = 0;

template <class mint>
void butterfly(std::vector<mint>& a) {
    int n = int(a.size());
    int h = __builtin_ctz((unsigned int)n);

    static const fft64_info<mint> info;
    int len = 0;  // a[i, i+(n>>len), i+2*(n>>len), ..] is transformed
    while (len < h) {
        if (h - len == 1) {
            int p = 1 << (h - len - 1);
            mint rot = 1;
            for (int s = 0; s < (1 << len); s++) {
                int offset = s << (h - len);
                for (int i = 0; i < p; i++) {
                    auto l = a[i + offset];
                    auto r = a[i + offset + p] * rot;
                    a[i + offset] = l + r;
                    a[i + offset + p] = l - r;
                }
                if (s + 1 != (1 << len)) {
                    rot *= info.rate2[__builtin_ctz(~(unsigned int)(s))];
                }
            }
            len++;
        } else {
            // 4-base
            int p = 1 << (h - len - 2);
            mint rot = 1, imag = info.root[2];
            for (int s = 0; s < (1 << len); s++) {
                mint rot2 = rot * rot;
                mint rot3 = rot2 * rot;
                int offset = s << (h - len);
                for (int i = 0; i < p; i++) {
                    auto a0 = a[i + offset];
                    auto a1 = a[i + offset + p] * rot;
                    auto a2 = a[i + offset + 2 * p] * rot2;
                    auto a3 = a[i + offset + 3 * p] * rot3;
                    auto a1na3imag = (a1 - a3) * imag;
                    a[i + offset] = a0 + a2 + a1 + a3;
                    a[i + offset + 1 * p] = a0 - a1 + a2 - a3;
                    a[i + offset + 2 * p] = a0 - a2 + a1na3imag;
                    a[i + offset + 3 * p] = a0 - a2 - a1na3imag;
                }
                if (s + 1 != (1 << len)) {
                    rot *= info.rate3[__builtin_ctz(~(unsigned int)(s))];
                }
            }
            len += 2;
        }
    }
}

template <class mint>
void butterfly_inv(std::vector<mint>& a) {
    int n = int(a.size());
    int h = __builtin_ctz((unsigned int)n);

    static const fft64_info<mint> info;

    int len = h;  // a[i, i+(n>>len), i+2*(n>>len), ..] is transformed
    while (len) {
        if (len == 1) {
            int p = 1 << (h - len);
            mint irot = 1;
            for (int s = 0; s < (1 << (len - 1)); s++) {
                int offset = s << (h - len + 1);
                for (int i = 0; i < p; i++) {
                    auto l = a[i + offset];
                    auto r = a[i + offset + p];
                    a[i + offset] = l + r;
                    a[i + offset + p] = (l - r) * irot;
                }
                if (s + 1 != (1 << (len - 1))) {
                    irot *= info.irate2[__builtin_ctz(~(unsigned int)(s))];
                }
            }
            len--;
        } else {
            // 4-base
            int p = 1 << (h - len);
            mint irot = 1, iimag = info.iroot[2];
            for (int s = 0; s < (1 << (len - 2)); s++) {
                mint irot2 = irot * irot;
                mint irot3 = irot2 * irot;
                int offset = s << (h - len + 2);
                for (int i = 0; i < p; i++) {
                    auto a0 = a[i + offset + 0 * p];
                    auto a1 = a[i + offset + 1 * p];
                    auto a2 = a[i + offset + 2 * p];
                    auto a3 = a[i + offset + 3 * p];
                    auto a2na3iimag = (a2 - a3) * iimag;
                    a[i + offset] = a0 + a1 + a2 + a3;
                    a[i + offset + 1 * p] = (a0 - a1 + a2na3iimag) * irot;
                    a[i + offset + 2 * p] = (a0 + a1 - a2 - a3) * irot2;
                    a[i + offset + 3 * p] = (a0 - a1 - a2na3iimag) * irot3;
                }
                if (s + 1 != (1 << (len - 2))) {
                    irot *= info.irate3[__builtin_ctz(~(unsigned int)(s))];
                }
            }
            len -= 2;
        }
    }
}

template <class mint>
std::vector<mint> convolution_naive(const std::vector<mint>& a, const std::vector<mint>& b) {
    int n = int(a.size()), m = int(b.size());
    std::vector<mint> ans(n + m - 1);
    if (n < m) {
        for (int j = 0; j < m; j++) {
            for (int i = 0; i < n; i++) {
                ans[i + j] += a[i] * b[j];
            }
        }
    } else {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                ans[i + j] += a[i] * b[j];
            }
        }
    }
    return ans;
}

#include "../modint/modint64_static.hpp"

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

template <unsigned long long mod = 998244353, class T>
std::vector<T> convolution_mod(const std::vector<T>& a, const std::vector<T>& b) {
    int n = int(a.size()), m = int(b.size());
    if (!n || !m) return {};
    using mint = modint64_static<mod>;
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