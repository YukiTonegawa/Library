#ifndef _ARITHMETIC_H_
#define _ARITHMETIC_H_
#include <vector>
#include "gcd.hpp"

template<typename T>
struct arithmetic_sequence {
    using seq = arithmetic_sequence<T>;
    static constexpr T inf = std::numeric_limits<T>::max();
    static constexpr T minf = std::numeric_limits<T>::min();

    T a, b; // ax + b

    T kth(T x) {
        return a * x + b;
    }

    // A_l + A_{l+1} + ... A_{r-1}
    T sum(T l, T r) {
        if (l >= r) return 0;
        return a * (r * (r - 1) - l * (l - 1)) / 2 + b * (r - l);
    }

    // {値が最小になるx, その時の値}
    std::pair<T, T> min(T l, T r) {
        assert(l < r);
        if (a > 0) return kth(l);
        else return kth(r - 1);
    }

    // {値が最大になるx, その時の値}
    std::pair<T, T> max(T l, T r) {
        assert(l < r);
        if (a > 0) return kth(r - 1);
        else return kth(l);
    }

    // 値が [l, r)に収まる添え字の範囲 [lx, rx)
    // 無数に存在する -> {minf, inf}
    // 0通り -> {inf, inf}
    // それ以外 -> {lx, rx}
    std::pair<T, T> index_range(T l, T r) {
        r--;
        if (l >= r) return {inf, inf};
        if (a == 0) {
            if (l <= b && b <= r) {
                return {minf, inf};
            }
            return {inf, inf};
        } 
        l -= b;
        r -= b;
        if (a < 0) {
            a = -a;
            l = -l;
            r = -r;
            std::swap(l, r);
        }
        T L = l / a + (l > 0 && l % a != 0 ? 1 : 0);
        T R = (r - 1) / a + (r <= 0 && (r - 1) / a != 0 ? -1 : 0);
        return {L, R + 1};
    }

    // a = 0  の場合 b < y ? inf : minf)
    // a >= 0 の場合 y以上になる最小インデックス
    // a < 0  の場合 y以上になる最大インデックス
    T lower_bound(T y) {
        if (a == 0) return b < y ? inf : minf;
        y -= b;
        if (a > 0) {
            return (y - 1) / a + (y <= 0 && (y - 1) / a != 0 ? 0 : 1);
        } else {
            // -a, -y 以下になる最大インデックス
            a = -a;
            y = -y;
            return y / a + (y < 0 && y % a != 0 ? -1 : 0);
        }
    }

    // a = 0  の場合 b <= y ? inf : minf)
    // a >= 0 の場合 yより大きい最小インデックス
    // a < 0  の場合 yより大きい最大インデックス
    T upper_bound(T y) {
        assert(y != inf);
        return lower_bound(y + 1);
    }

    // ax + bがcの倍数になるようなxは大体の等差数列になる (これを返す)
    // ax ≡ -b mod c
    // 解がない場合 -> {inf, inf}
    // (mod c上で)一意に定まる -> {0, 最小の非負整数}
    // 複数ある   -> {最小の自然数, 最小の非負整数}
    // c <= 10^9
    seq multiple(long long c) {
        assert(c > 0);
        a = safe_mod(a, c);
        b = safe_mod(-b, c);
        if (a == 0) {
            if (b == 0) return {1, 0};
            else return {inf, inf};
        }
        // ax ≡ g
        auto [g, x] = inv_gcd(a, c);
        if (b % g) return {inf, inf};
        x = x * (b / g) % c;
        // ax ≡ -b
        // a((任意の整数) * (c / g) + x) ≡ -b
        T t = c / g;
        T z = x % t;
        return seq{t, z};
    }

    // 2つの等差数列の共通項は等差数列になる
    // 共通部分がない -> {inf, inf}
    // 一意に定まる   -> {0, ?}
    // 複数ある    -> {最小の自然数, 最小の非負整数}
    // a, b, c, d <= 10^9
    seq intersection(seq r) {
        auto [c, d] = r;
        assert(a != inf && c != inf);
        // ax + b = cy + d
        // -> ax + b - d がcの倍数
        // -> ax ≡ d - b (mod c)
        a = abs(a);
        b = abs(b);
        if (a > b) {
            std::swap(a, c);
            std::swap(b, d);
        }
        if (c == 0){
            if (b == d) return {0, d};
            return {inf, inf};
        }
        auto s = seq{a, b - d}.multiple(c);
        if (s.a == inf) return {inf, inf};
        // x = e*(任意の整数)+f
        s.a *= a;
        s.b = s.b * a + b;
        return seq{s.a, (s.a ? s.b % s.a : s.b)};
    }
};
#endif