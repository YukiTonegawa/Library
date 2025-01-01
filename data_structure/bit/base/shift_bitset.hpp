#ifndef _SHIFT_BITSET_H_
#define _SHIFT_BITSET_H_
#include <algorithm>

template<typename Vec, typename Int>
void lshift_bitset(Vec &v, int s, int max_block) {
    static constexpr int len_block = sizeof(Int) * 8;
    const int L = s / len_block;
    const int rem = s % len_block;
    v.resize(std::min((int)v.size() + (s + len_block - 1) / len_block, max_block), 0);
    int M = v.size();
    for (int b = M - L - 1; b >= 0; b--) {
        if (b != M - L - 1 && rem) {
            v[b + L + 1] |= v[b] >> (len_block - rem);
        }
        v[b + L] = v[b] << rem;
    }
    std::fill(v.begin(), v.begin() + std::min((int)v.size(), L), 0);
}

template<typename Vec, typename Int>
void rshift_bitset(Vec &v, int s) {
    static constexpr int len_block = sizeof(Int) * 8;
    const int M = v.size();
    const int L = s / len_block;
    const int rem = s % len_block;
    for (int b = L; b < M; b++) {
        if (b != L && rem) {
            v[b - L - 1] |= v[b] << (len_block - rem);
        }
        v[b - L] = v[b] << rem;
    }
    v.resize(M - L);
}
#endif