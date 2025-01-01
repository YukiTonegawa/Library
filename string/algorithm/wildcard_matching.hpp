#ifndef _WILDCARD_PATTERN_MATCHING_H_
#define _WILDCARD_PATTERN_MATCHING_H_
#include <vector>
#include <algorithm>
#include <cassert>
#include "../../math/fft/convolution_mod.hpp"
#include "../../math/fft/convolution64.hpp"

// res[i] := aのiから始めてパターンbがマッチするか([0, n - m])
// a, b中の0はワイルドカード
// 0 <= ai <= 10^6
// 0 <= bi <= 10^6
std::vector<bool> wildcard_matching(std::vector<long long> a, std::vector<long long> b) {
    int n = a.size();
    int m = b.size();
    assert(n >= m);
    std::vector<bool> res(n - m + 1, 1);
    std::reverse(b.begin(), b.end());
    std::vector<long long> aa(n), aflag(n), bb(m), bflag(m);
    int elem_limit = 1000000;
    for (int i = 0; i < n; i++) {
        assert(0 <= a[i] && a[i] <= elem_limit);
        aa[i] = a[i] * a[i];
        aflag[i] = (a[i] != 0);
    }
    for (int i = 0; i < m; i++) {
        assert(0 <= a[i] && a[i] <= elem_limit);
        bb[i] = b[i] * b[i];
        bflag[i] = (b[i] != 0);
    }
    auto ab = convolution64(a, b);
    aa = convolution64(aa, bflag);
    bb = convolution64(bb, aflag);
    for (int i = m - 1; i < n; i++) res[i - (m - 1)] = (aa[i] - 2 * ab[i] + bb[i] == 0);
    return res;
}

// (値の最大値)^2 * max(|a|, ||b)が998244353未満
std::vector<bool> wildcard_matching_small(const std::vector<int> &A, const std::vector<int> &B) {
    int N = A.size(), M = B.size(), K = bit_ceil(N + M - 1);
    std::vector<bool> res(N - M + 1);
    std::vector<modint998244353> a(K, 0), b(K, 0), aa(K, 0), bb(K, 0), aflag(K, 0), bflag(K, 0);
    for (int i = 0; i < N; i++) {
        a[i] = A[i];
        aa[i] = a[i] * a[i];
        aflag[i] = (a[i] != 0);
    }
    for (int i = 0; i < M; i++) {
        b[i] = B[M - 1 - i];
        bb[i] = b[i] * b[i];
        bflag[i] = (b[i] != 0);
    }
    butterfly(a);
    butterfly(b);
    butterfly(aa);
    butterfly(bb);
    butterfly(aflag);
    butterfly(bflag);
    for (int i = 0; i < K; i++) {
        a[i] = aa[i] * bflag[i] - 2 * a[i] * b[i] + bb[i] * aflag[i];
    }
    butterfly_inv(a);
    for (int i = M - 1; i < N; i++) res[i - (M - 1)] = a[i] == 0;
    return res;
}
#endif