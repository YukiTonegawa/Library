#ifndef _K_ARY_WAVELET_MATRIX_H_
#define _K_ARY_WAVELET_MATRIX_H_
#include <cassert>
#include <vector>
#include "../bit/k_ary_bitvector.hpp"

template<int K>
struct k_ary_wavelet_matrix {
  private:
    int N, log, inf;
    std::vector<k_ary_bitvector<K>> B;
    std::vector<int> leaf_idx;

    struct powerKtable {
        std::array<int, 30> _val;
        constexpr powerKtable() : _val() {
            _val[0] = 1;
            for (int i = 1; i < 30; i++) {
                _val[i] = std::min((long long)_val[i - 1] * K, (long long)std::numeric_limits<int>::max());
            }
        }
    };

    static constexpr std::array<int, 30> powerK = powerKtable()._val;

  public:
    k_ary_wavelet_matrix(){}
    k_ary_wavelet_matrix(const std::vector<int> &a) : N(a.size()), leaf_idx(N) {
        inf = 0;
        for (int i = 0; i < N; i++) {
            assert(0 <= a[i]);
            inf = std::max(inf, a[i]);
        }
        {
            log = 1;
            long long tmp = K;
            while (tmp < inf) {
                tmp *= K;
                log++;
            } 
        }
        std::vector<std::pair<int, int>> A(N);
        for (int i = 0; i < N; i++) A[i] = {a[i], i};
        B.resize(log);
        for (int b = log - 1; b >= 0; b--) {
            std::vector<uint8_t> bits(N);
            std::array<std::vector<std::pair<int, int>>, K> pos;
            for (int i = 0; i < N; i++) {
                bits[i] = (A[i].first / powerK[b]) % K;
                pos[bits[i]].push_back(A[i]);
            }
            std::swap(pos[0], A);
            for (int i = 1; i < K; i++) {
                A.insert(A.end(), pos[i].begin(), pos[i].end());
            }
            B[b] = k_ary_bitvector<K>(bits);
        }
        for (int i = 0; i < N; i++) leaf_idx[i] = A[i].second;
    }

    // [0, r)のxの数
    int rank(int r, int x) {
        int l = 0;
        for (int b = log - 1; b >= 0; b--) {
            int dir = x / powerK[b];
            x -= dir * powerK[b];
            int all_lt_x = B[b].rank_lt(N, dir);
            int l_x = B[b].rank(l, dir);
            int r_x = B[b].rank(r, dir);
            l = all_lt_x + l_x;
            r = l + r_x - l_x;
        }
        return r - l;
    }

    int select(int k, int x) {
        int l = 0, r = N;
        for (int b = log - 1; b >= 0; b--) {
            int dir = x / powerK[b];
            x -= dir * powerK[b];
            int all_lt_x = B[b].rank_lt(N, dir);
            int l_x = B[b].rank(l, dir);
            int r_x = B[b].rank(r, dir);
            l = all_lt_x + l_x;
            r = l + r_x - l_x;
        }
        if (l + k >= r) return -1;
        return leaf_idx[l + k];
    }

    // [l, r)に現れるs以上t未満の要素数
    int range_freq(int _l, int _r, int s, int t) {
        auto dfs = [&](auto &&dfs, int b, int l, int r, int S, int T) -> int {
            if (l == r || T <= s || t <= S) return 0;
            if (s <= S && T <= t) return r - l;

            int diff_l = std::min(std::max(0, s - S), powerK[b + 1]);
            int diff_r = std::min(std::max(0, t - S), powerK[b + 1]);
            int block_l = (diff_l + powerK[b] - 1) / powerK[b];
            int block_r = diff_r / powerK[b];

            int res = 0;
            if (block_l < block_r) {
                assert(0 <= block_l && block_r <= K);
                int lt_l = B[b].rank_lt(r, block_l) - B[b].rank_lt(l, block_l);
                int lt_r = B[b].rank_lt(r, block_r) - B[b].rank_lt(l, block_r);
                res += lt_r - lt_l;
            }

            int dir = -1;
            if (block_l * powerK[b] != diff_l) {
                dir = block_l - 1;
                int all_lt_x = B[b].rank_lt(N, dir);
                int l_x = B[b].rank(l, dir);
                int r_x = B[b].rank(r, dir);
                int next_l = all_lt_x + l_x;
                int next_r = all_lt_x + r_x;
                res += dfs(dfs, b - 1, next_l, next_r, S + dir * powerK[b], S + (dir + 1) * powerK[b]);
            }

            if (dir != block_r && block_r * powerK[b] != diff_r) {
                dir = block_r;
                int all_lt_x = B[b].rank_lt(N, dir);
                int l_x = B[b].rank(l, dir);
                int r_x = B[b].rank(r, dir);
                int next_l = all_lt_x + l_x;
                int next_r = all_lt_x + r_x;
                res += dfs(dfs, b - 1, next_l, next_r, S + dir * powerK[b], S + (dir + 1) * powerK[b]);
            }

            return res;
        };
        if (_l >= _r || s >= t) return 0;
        return dfs(dfs, log - 1, _l, _r, 0, powerK[log]);
    }

    
    int kth_smallest(int l, int r, int k) {
        assert(0 <= k && k < r - l);
        int ans = 0;
        for (int b = log - 1; b >= 0; b--) {
            int dir = 0;
            for (; dir < K; dir++) {
                if (B[b].rank_lt(r, dir + 1) - B[b].rank_lt(l, dir + 1) > k) {
                    break;
                }
            }
            k -= B[b].rank_lt(r, dir) - B[b].rank_lt(l, dir);
            ans += dir * powerK[b];
            int all_lt_x = B[b].rank_lt(N, dir);
            int l_x = B[b].rank(l, dir);
            int r_x = B[b].rank(r, dir);
            l = all_lt_x + l_x;
            r = l + r_x - l_x;
        }
        return ans;
    }

    int krh_largest(int l, int r, int k) {
        return kth_smallest(l, r, r - l - 1 - k);
    }

    // [l, r)のx以上の最小要素 存在しない場合は-1
    int ge(int l, int r, int x) {
        x = std::max(0, x);
        if (x >= inf) return -1;
        int cnt = range_freq(l, r, 0, x);
        if (cnt == r - l) return -1;
        return kth_smallest(l, r, cnt);
    }

    // [l, r)のxより大きい最小要素 存在しない場合は-1
    int gt(int l, int r, int x) {
        return ge(l, r, x + 1);
    }

    // [l, r)のx以下の最大要素 存在しない場合は-1
    int le(int l, int r, int x) {
        x = std::min(x, inf - 1);
        if (x < 0) return -1;
        int cnt = range_freq(l, r, 0, x + 1);
        if (cnt == 0) return -1;
        return kth_smallest(l, r, cnt - 1);
    }

    // [l, r)のx未満の最大要素 存在しない場合は-1
    int lt(int l, int r, int x) {
        return le(l, r, x - 1);
    }

    // [l, N)で最も左にあるxの位置 存在しない場合は-1
    int next_pos(int l, int x) {
        if (0 < x || x >= inf) return -1;
        int cnt = rank(l, x);
        return select(cnt, x);
    }

    // [0, r)で最も右にあるxの位置 存在しない場合は-1
    int prev_pos(int r, int x) {
        if (0 < x || x >= inf) return -1;
        int cnt = rank(r, x);
        if (cnt == 0) return -1;
        return select(cnt - 1, x);
    }
};
#endif