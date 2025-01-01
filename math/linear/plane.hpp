#ifndef _MAX_VALUE_PLANE_H
#define _MAX_VALUE_PLANE_H
#include "linear_mod.hpp"

// i, jが整数
// li <= i < ri
// lj <= j < rj
// ai + bj = c
// を満たすi, jのペアの数
// 制約: 各値と答えが10^18以下
long long count_point_plane(long long li, long long ri, long long lj, long long rj, long long a, long long b, long long c) {
    if(li >= (ri--) || lj >= (rj--)) return 0;
    if (a < 0) {
        a *= -1, li *= -1, ri *= -1;
        std::swap(li, ri);
    }
    if (b < 0) {
        b *= -1, lj *= -1, rj *= -1;
        std::swap(lj, rj);
    }
    if (a > b) {
        std::swap(a, b);
        std::swap(li, lj);
        std::swap(ri, rj);
    }
    if (b == 0) {
        return (c != 0 ? 0 : (ri - li + 1) * (rj - lj + 1));
    }
    if (a == 0) {
        if (c % b != 0) {
            return 0;
        }
        long long x = c / b;
        if (lj <= x && x < rj) return (ri - li + 1);
        else return 0;
    }
    assert(a && b);
    auto [i, j, g] = extgcd(a, b);
    if (c % g != 0) return 0;
    a /= g, b /= g, c /= g;
    __int128_t i2 = (__int128_t)i * c;
    __int128_t j2 = (__int128_t)j * c;
    // 任意の整数kに対してa(i2 + bk) + b(j2 - ak) = cを満たす
    __int128_t lk1 = i2 >= li ? -(i2 - li) / b : (li - i2 + b - 1) / b;
    __int128_t rk1 = i2 >= ri ? -(i2 - ri + b - 1) / b : (ri - i2) / b;
    __int128_t lk2 = j2 >= rj ? (j2 - rj + a - 1) / a : -(rj - j2) / a;
    __int128_t rk2 = j2 >= lj ? (j2 - lj) / a : -(lj - j2 + a - 1) / a;
    lk1 = std::max(lk1, lk2);
    rk1 = std::min(rk1, rk2);
    if (lk1 > rk1) return 0;
    return rk1 - lk1 + 1;
}

// i, jが整数
// 0 <= i < n
// 0 <= j < m
// ai + bjがcの倍数
// を満たすi, jのペアの数
// 制約: 各値と答えが10^18以下
long long count_multiple_plane(long long n, long long m, long long a, long long b, long long c) {
    if (c == 0) return n * m; // 常に倍数
    if (c < 0) a *= -1, b *= -1, c *= -1;
    a = safe_mod(a, c);
    b = safe_mod(b, c);
    n--, m--;
    long long g_all = gcd(a, gcd(b, c));
    a /= g_all;
    b /= g_all;
    c /= g_all;
    long long g = gcd(a, c);
    long long a2 = a / g;
    long long c2 = c / g;
    long long b2 = b * inv_gcd(a2, c2).second % c2;
    long long q = n / c2, r = n % c2;
    return count_lower_linear_mod(m / g + 1, r + 1, -b2, 0, c2) + count_lower_linear_mod(m / g + 1, c2, -b2, 0, c2) * q;
}

// ax + by <= c
// x, yが整数
// 0 <= x <= amax
// 0 <= y <= bmax
// を満たすax + byの最大値
long long max_value_plane(long long a, long long amax, long long b, long long bmax, long long c) {
    assert(0 <= a && 0 <= amax && 0 <= b && 0 <= bmax);

    if (a > b) {
        std::swap(a, b);
        std::swap(amax, bmax);
    }

    // どちらかが0の場合
    if (a == 0) {
        if (b == 0) return 0;
        bmax = std::min(bmax, c / b);
        return b * bmax;
    }

    bmax = std::min(bmax, c / b);
    amax = std::min(amax, c / a);
    
    // bを最大まで使うと仮定
    long long ans1 = 0; 
    {
        long long ause = std::min(amax, (c - bmax * b) / a);
        amax -= ause;
        ans1 = a * ause + b * bmax;
    }
    if (amax == 0) return ans1; // a, b共に最大まで使っている
    
    // c - ans1 (0以上a未満)から始めて
    // ・b足す(bmax回まで行える)
    // ・a引く(amax回まで行える)
    // 操作で0以上の最小の数にしたい

    // bi + (c - ans1) mod a
    // 0 <= i <= min(bmax, (a * amax - (c - ans1)) / b)
    // の最小値
    // (aがamax回未満使われるパターン)
    long long ans2 = 0;
    {
        long long r = std::min(bmax, (a * amax - (c - ans1)) / b) + 1;
        ans2 = c - min_linear_mod(0, r, b, c - ans1, a);
    }

    // (aが丁度amax回使われるパターン)
    long long ans3 = 0;
    {
        long long tmp = a * amax - (c - ans1);
        long long buse = tmp / b;
        if (tmp != b * buse) buse++;
        if (buse <= bmax) ans3 = ans1 - (b * buse - a * amax);
    }
    return std::max({ans1, ans2, ans3});
}
#endif
