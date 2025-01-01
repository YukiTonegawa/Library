#ifndef _NAZOKI_H_
#define _NAZOKI_H_
#include <vector>
#include <cassert>
#include <array>
#include <algorithm>
#include <string>

// [0, 64^D)
template<int D>
struct nazotree {
  private:
    static_assert(1 <= D && D <= 4); // 64, 4096, 262144, 16777216
    using ull = uint64_t;
    static constexpr int len_block = 64;
    static constexpr int node_count = (1 << (6 * D)) / (len_block - 1); // 1 + 64 + 64^2 ... 64^(D-1)
    static constexpr int inf = 1 << (6 * D);
    std::array<ull, node_count> flag;

  public:
    nazotree() { flag.fill(0); }
    nazotree(const std::string &s, char one = '1') {
        flag.fill(0);
        int n = std::min((int)s.size(), 1 << (6 * D));
        for (int i = 0; i < n; i++) {
            if (s[i] == one) {
                int j = (i + node_count - 1);
                flag[j / len_block] |= 1ULL << (j % len_block);
            }
        }
        for (int i = node_count - 1; i > 0; i--) {
            if (flag[i]) {
                flag[(i - 1) / len_block] |= 1ULL << ((i - 1) % len_block);
            }
        }
    }

    // kを追加. ある場合は何もしない
    void insert(int k) {
        k += node_count;
        while (k--) {
            int dir = k % len_block;
            k /= len_block;
            flag[k] |= 1ULL << dir;
        }
    }

    // kを削除. ない場合は何もしない
    void erase(int k) {
        k += node_count;
        bool f = true;
        while (k--) {
            int dir = k % len_block;
            k /= len_block;
            flag[k] &= ~((ull)f << dir);
            f = !flag[k];
        }
    }
  
    bool find(int k) {
        k += node_count;
        int p0 = (k - 1) / len_block;
        int d0 = (k - 1) % len_block;
        return (flag[p0] >> d0) & 1;
    }

    // k以上の最小要素, 存在しない場合-1
    int ge(int k) {
        if (k >= inf) return -1;
        k = std::max(k, 0);
        if (find(k)) return k;
        k += node_count;
        while(k--) {
            int dir = k % len_block;
            k /= len_block;
            ull f = (flag[k] >> dir) >> 1; // dirより大きい1が存在する
            if (f) {
                k = (k * len_block) + dir + 2 + __builtin_ctzll(f);
                while(k < node_count) {
                    k = (k * len_block) + __builtin_ctzll(flag[k]) + 1;
                }
                return k - node_count;
            }
        }
        return -1;
    }

    int gt(int k) {
        return ge(k + 1);
    }

    // k以下の最大要素, 存在しない場合-1
    int le(int k) {
        if (k < 0) return -1;
        k = std::min(k, inf - 1);
        if(find(k)) return k;
        k += node_count;
        while (k--) {
            int dir = k % len_block;
            k /= len_block;
            ull f = flag[k] & ((1ULL << dir) - 1); // dirより小さい1が存在する
            if (f) {
                k = (k * len_block) + len_block - __builtin_clzll(f);
                while(k < node_count) {
                    k = (k * len_block) + len_block - __builtin_clzll(flag[k]);
                }
                return k - node_count;
            }
        }
        return -1;
    }

    int lt(int k) {
        return le(k - 1);
    }
};

#endif