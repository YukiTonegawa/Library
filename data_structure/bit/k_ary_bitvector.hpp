#ifndef _K_ARY_BITVECTOR_H_
#define _K_ARY_BITVECTOR_H_
#include <vector>
#include <array>

template<int K>
struct k_ary_bitvector {
  private:
    using Uint = uint64_t;
    static constexpr int len_block = sizeof(Uint) * 8;
    int M;
    std::vector<std::array<int, K>> Popsum;
    std::vector<std::array<int, K + 1>> Popsum_lt;
    std::vector<std::array<Uint, K>> Block;
    std::vector<std::array<Uint, K + 1>> Block_lt;

  public:
    k_ary_bitvector(): M(0) {}
    k_ary_bitvector(const std::vector<uint8_t> &v): M((v.size() + len_block - 1) / len_block), 
    Popsum(M + 1), Popsum_lt(M + 1), Block(M), Block_lt(M) {
        Popsum[0].fill(0);
        for (int b = 0; b < M; b++) {
            int l = b * len_block;
            int r = std::min(l + len_block, (int)v.size());
            Block[b].fill(0);
            for (int j = l; j < r; j++) {
                assert(0 <= v[j] && v[j] < K);
                Popsum[b + 1][v[j]]++;
                Block[b][v[j]] |= Uint(1) << (j - l);
            }
            for (int j = 0; j < K; j++) {
                Popsum[b + 1][j] += Popsum[b][j];
            }
        }
        Popsum_lt[0].fill(0);
        for (int i = 1; i <= M; i++) {
            Popsum_lt[i][0] = 0;
            for (int j = 1; j <= K; j++) {
                Popsum_lt[i][j] = Popsum_lt[i][j - 1] + Popsum[i][j - 1];
            }
        }
        for (int i = 0; i < M; i++) {
            Block_lt[i][0] = 0;
            for (int j = 1; j <= K; j++) {
                Block_lt[i][j] = Block_lt[i][j - 1] + Block[i][j - 1];
            }
        }
    }

    // [0, r)のxの数
    int rank(int r, int x) {
        int q = r / len_block;
        r -= q * len_block;
        return Popsum[q][x] + (r ? __builtin_popcountll(Block[q][x] << (len_block - r)) : 0);
    }

    // [0, r)のt未満の数
    int rank_lt(int r, int t) {
        int q = r / len_block;
        r -= q * len_block;
        return Popsum_lt[q][t] + (r ? __builtin_popcountll(Block_lt[q][t] << (len_block - r)) : 0);
    }
};
#endif
