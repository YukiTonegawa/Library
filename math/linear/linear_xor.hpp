#ifndef _LINEAR_XOR_H_
#define _LINEAR_XOR_H_
#include <cassert>
#include <cstdint>

// 0 ^ 1 ^ 2 ... (r-1)
long long accumulate_xor(long long r) {
    if (--r <= 0) return 0;
    if (r & 1) return r % 4 == 1;
    return (r % 4 == 0 ? r : r + 1);
}

// b ^ (a + b) ^ (2a + b).....((n - 1) * a + b)
// n, a, b <= 10^9
long long linear_xor(long long n, long long a, long long b) {
    assert(0 < n && 0 <= a && 0 <= b);
    long long res = 0, max_y = (n - 1) * a + b;
    int max_bit = (max_y == 0 ? 0 : 63 - __builtin_clzll(max_y));

    auto fsum = [&] (auto &&fsum, long long N, long long A, long long B, long long C) -> bool {
        bool ans = 0;
        if (A >= C) {
            ans ^= ((((N - 1) * N) >> 1) & (A / C)) & 1;
            A %= C;
        }
        if (B >= C) {
            ans ^= (N & (B / C)) & 1;
            B %= C;
        }
        long long y_max = A * N + B;
        if (y_max >= C) ans ^= fsum(fsum, y_max / C, C, y_max % C, A);
        return ans;
    };

    for (int i = max_bit; i >= 0; i--) res |= ((long long)fsum(fsum, n, a, b, 1LL << i)) << i;
    return res;
}

// n, a, b <= 10^9
long long linear_or(long long n, long long a, long long b) {
    assert(0 < n && 0 <= a && 0 <= b);
    long long res = 0, max_y = (n - 1) * a + b;
    int max_bit = (max_y == 0 ? 0 : 63 - __builtin_clzll(max_y));
    for (int i = 0; i <= max_bit; i++) {
        if (((b | max_y) >> i) & 1) {
            res |= 1ULL << i;
            continue;
        }
        long long mask = (1ULL << (i + 1)) - 1; // [0, i]bit
        long long a2 = a & mask, x = b & mask, y = max_y & mask;
        if (a2 < (1ULL << i)) {
            // y - x = (n - 1) * a2なら0
            if (y - x != __int128_t(n - 1) * a2) {
                res |= 1ULL << i;
            }
        }else{
            // x - sub * (n - 1) == yなら0
            long long sub = (1ULL << (i + 1)) - a2;
            if (x != y + sub * __int128_t(n - 1)) {
                res |= 1ULL << i;
            }
        }
    }
    return res;
}

long long linear_and(long long n, long long a, long long b) {
    assert(0 < n && 0 <= a && 0 <= b);
    long long res = 0, max_y = (n - 1) * a + b;
    int max_bit = (b == 0 ? 0 : 63 - __builtin_clzll(b));
    for (int i = 0; i <= max_bit; i++) {
        if ((((b & max_y) >> i) & 1) == 0) continue;
        long long mask = (1ULL << (i + 1)) - 1; // [0, i]bit
        long long a2 = a & mask, x = b & mask, y = max_y & mask;
        if (a2 < (1ULL << i)) {
            // y - x = (n - 1) * a2なら1
            if (y - x == __int128_t(n - 1) * a2) {
                res |= 1ULL << i;
            }
        }else{
            // x - sub * (n - 1) == yなら1
            long long sub = (1ULL << (i + 1)) - a2;
            if (x == y + sub * __int128_t(n - 1)) {
                res |= 1ULL << i;
            }
        }
    }
    return res;
}
#endif