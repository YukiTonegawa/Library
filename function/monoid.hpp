#ifndef _MONOID_H_
#define _MONOID_H_
#include <limits>
#include <array>
#include <numeric>
#include <algorithm>
#include "function.hpp"

template<typename T>
struct range_sum {
    using S = T;
    static constexpr S e() { return 0; }
    static constexpr S op(S a, S b) { return a + b; }
    static constexpr S flip(S a) { return a; }
};
template<typename T>
struct range_min {
    using S = T;
    static constexpr S e() { return func::get_inf<S>(); }
    static constexpr S op(S a, S b) { return std::min(a, b); }
    static constexpr S flip(S a) { return a; }
};
template<typename T>
struct range_max {
    using S = T;
    static constexpr S e() { return func::get_minf<S>(); }
    static constexpr S op(S a, S b) { return std::max(a, b); }
    static constexpr S flip(S a) { return a; }
};
template<typename T>
struct range_mul {
    using S = T;
    static constexpr S e() { return 1; }
    static constexpr S op(S a, S b) { return a * b; }
};
// [0, K]番目に小さい要素を取得できる
template<typename T, int K>
struct range_kth_min {
    using S = std::array<T, K + 1>;
    static constexpr T inf = func::get_inf<T>();
    static constexpr S e() { S c; c.fill(inf); return c; }
    static constexpr S op(S a, S b) {
        S c;
        for (int i = 0, j = 0, k = 0; k <= K; k++) {
            c[k] = (a[i] < b[j] ? a[i++] : b[j++]);
        }
        return c;
    }
};
// [0, K]番目にに小さい要素の{値, 個数}
template<typename T, int K>
struct range_kth_min_count {
    static_assert(K >= 0);
    using S = std::array<std::pair<T, int>, K + 1>;
    static constexpr T inf = func::get_inf<T>();
    static constexpr S e() { S c; c.fill({inf, 0}); return c; }
    static constexpr S op(S a, S b) {
        S c;
        c.fill({inf, 0});
        int i = 0, j = 0, k = 0;
        while (k <= K && (i <= K || j <= K)) {
            if (j == K + 1 || (i <= K && a[i] <= b[j])) {
                c[k] = a[i++];
                if (j <= K && c[k].first == b[j]) c[k].second += b[j++].second;
                k++;
            } else {
                c[k++] = b[j++];
            }
        }
        return c;
    }
};
template<typename mint>
struct range_composite {
    using S = std::pair<mint, mint>;
    static constexpr S e() { return {1, 0}; }
    static constexpr S op(S a, S b) { return {a.first * b.first, a.second * b.first + b.second}; }
};
template<int mod>
struct range_composite_int {
    using S = std::pair<int, int>;
    static constexpr S e() { return {1, 0}; }
    static constexpr S op(S a, S b) {
        return {(long long)a.first * b.first % mod, ((long long)a.second * b.first + b.second) % mod};
    }
};
template<typename T>
struct range_add_range_sum {
    using S = T;
    using F = T;
    static constexpr S e() { return 0; }
    static constexpr F id() { return 0; }
    static constexpr S op(S a, S b) { return a + b; }
    static constexpr S mapping(F a, S b, int c) { return b + a * c; }
    static constexpr F composition(F a, F b) { return a + b; }
    static constexpr S flip(S a) { return a; }
};
template<typename T>
struct range_chmin_range_min {
    using S = T;
    using F = T;
    static constexpr S e() { return func::get_inf<S>(); }
    static constexpr F id() { return func::get_inf<S>(); }
    static constexpr S op(S a, S b) { return std::min(a, b); }
    static constexpr S mapping(F a, S b, int c) { return std::min(a, b); }
    static constexpr F composition(F a, F b) { return std::min(a, b); }
    static constexpr S flip(S a) { return a; }
};
template<typename T>
struct range_chmax_range_max {
    using S = T;
    using F = T;
    static constexpr S e() { return func::get_minf<S>(); }
    static constexpr F id() { return func::get_minf<S>(); }
    static constexpr S op(S a, S b) { return std::max(a, b); }
    static constexpr S mapping(F a, S b, int c) { return std::max(a, b); }
    static constexpr F composition(F a, F b) { return std::max(a, b); }
    static constexpr S flip(S a) { return a; }
};
template<typename mint>
struct range_affine_range_sum {
    using S = mint;
    using F = std::pair<mint, mint>;
    static constexpr S e() { return 0; }
    static constexpr F id() { return {1, 0}; }
    static constexpr S op(S a, S b) { return a + b; }
    static constexpr S mapping(F a, S b, int c) { return a.first * b + a.second * c; }
    static constexpr F composition(F a, F b) { return {a.first * b.first, a.first * b.second + a.second}; }
};
template<typename T>
struct range_add_range_min {
    using S = T;
    using F = T;
    static constexpr S e() { return func::get_inf<S>(); }
    static constexpr F id() { return 0; }
    static constexpr S op(S a, S b) { return std::min(a, b); }
    static constexpr S mapping(F a, S b, int c) { return b + (b == e() ? 0 : a); };
    static constexpr F composition(F a, F b) { return a + b; }
};
template<typename T>
struct range_add_range_max {
    using S = T;
    using F = T;
    static constexpr S e() { return func::get_minf<S>(); }
    static constexpr F id() { return 0; }
    static constexpr S op(S a, S b) { return std::max(a, b); }
    static constexpr S mapping(F a, S b, int c) { return b + (b == e() ? 0 : a); };
    static constexpr F composition(F a, F b) { return a + b; }
};
template<typename T>
struct range_add_range_argmin {
    using S = std::pair<T, int>;
    using F = T;
    static constexpr T inf = func::get_inf<T>();
    static constexpr S e() { return {inf, -1}; }
    static constexpr F id() { return 0; }
    static constexpr S op(S a, S b) { return std::min(a, b); }
    static constexpr S mapping(F a, S b, int c) {
        if (b.first == inf) return b;
        return {a.first + b, a.second};
    }
    static constexpr F composition(F a, F b) { return a + b; }
};
// [0, K]番目にに小さい要素の{値, 個数}
template<typename T, int K>
struct range_add_range_kth_min_count {
    static_assert(K >= 0);
    using S = std::array<std::pair<T, int>, K + 1>;
    using F = T;
    static constexpr T inf = func::get_inf<T>();
    static constexpr S e() { S c; c.fill({inf, 0}); return c; }
    static constexpr F id() { return 0; }
    static constexpr S op(S a, S b) {
        S c;
        c.fill({inf, 0});
        int i = 0, j = 0, k = 0;
        while (k <= K && (i <= K || j <= K)) {
            if (j == K + 1 || (i <= K && a[i] <= b[j])) {
                c[k] = a[i++];
                if (j <= K && c[k].first == b[j]) c[k].second += b[j++].second;
                k++;
            } else {
                c[k++] = b[j++];
            }
        }
        return c;
    }
    static constexpr S mapping(F a, S b, int c) {
        for (int i = 0; i <= K; i++) {
            if (b[i].first == inf) return b;
            b[i].first += a;
        }
        return b;
    }
    static constexpr F composition(F a, F b) {
        return a + b;
    }
};
#endif