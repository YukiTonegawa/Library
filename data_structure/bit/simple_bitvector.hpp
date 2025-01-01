#ifndef _SIMPLE_BITVECTOR_H_
#define _SIMPLE_BITVECTOR_H_
#include <vector>

struct simple_bitvector {
    using Uint = uint64_t;
    static constexpr int len_block = sizeof(Uint) * 8;
    int N, M;
    std::vector<Uint> Block;
    std::vector<int> Popsum;
    
    simple_bitvector() : N(0), M(0) {}
    simple_bitvector(const std::vector<bool> &v) : N(v.size()), M((v.size() + len_block - 1) / len_block), Block(M), Popsum(M + 1) {
        Popsum[0] = 0;
        for (int b = 0; b < M; b++) {
            int l = b * len_block;
            int r = std::min((int)v.size(), l + len_block);
            Uint x = 0;
            for (int j = l, s = 0; j < r; j++, s++) {
                x += Uint(v[j]) << s;
            }
            Block[b] = x;
            Popsum[b + 1] = Popsum[b] + __builtin_popcountll(x);
        }
    }

    bool access(int k) {
        int q = k / len_block;
        k -= q * len_block;
        return (Block[q] >> k) & 1;
    }

    // sum[0, r)
    int rank(int r) {
        int q = r / len_block;
        r -= q * len_block;
        return Popsum[q] + (r ? __builtin_popcountll(Block[q] << (len_block - r)) : 0);
    }

    int size() {
        return N;
    }
};
#endif