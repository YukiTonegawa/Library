#ifndef _WAVELET_TREE_DYNAMIC_H_
#define _WAVELET_TREE_DYNAMIC_H_
#include <numeric>
#include <queue>
#include "../../math/base/bit_ceil.hpp"
#include "../bit/dynamic_bitvector.hpp"

struct wavelet_tree_dynamic {
    int log, inf;
    std::vector<dynamic_bitvector> B;
    wavelet_tree_dynamic() {}
    wavelet_tree_dynamic(const std::vector<int> &a, int inf) : inf(inf) {
        int N = a.size();
        for (int i = 0; i < N; i++) {
            assert(0 <= a[i] && a[i] < inf);
        }
        log = std::max(1, bit_ceil_log(inf));
        B.resize(1 << log);

        std::vector<std::tuple<int, int, int>> A(N); // {ブロック, 値, 元のインデックス}
        for (int i = 0; i < N; i++) A[i] = {1, a[i], i};

        for (int b = log - 1; b >= 0; b--) {
            std::vector<std::tuple<int, int, int>> LB, RB;
            int l = 0;
            while (l < N) {
                int block = std::get<0>(A[l]);
                int r = l + 1;
                while (r < N && std::get<0>(A[r]) == block) {
                    r++;
                }
                std::vector<bool> bits(r - l);
                for (int i = l; i < r; i++) {
                    auto [bi, ai, idx] = A[i];
                    if (((ai >> b) & 1) == 0) {
                        LB.push_back({bi * 2, ai, idx});
                        bits[i - l] = 0;
                    } else {
                        RB.push_back({bi * 2 + 1, ai, idx});
                        bits[i - l] = 1;
                    }
                }
                B[block] = dynamic_bitvector(bits);
                LB.insert(LB.end(), RB.begin(), RB.end());
                RB.clear();
                l = r;
            }
            std::swap(LB, A);
        }
    }

    // build時にf({ブロック, 値, 元のidx}のvector)を呼び出す
    // ブロックは1-indexed
    template<typename F>
    wavelet_tree_dynamic(const std::vector<int> &a, int inf, F f) : inf(inf) {
        int N = a.size();
        for (int i = 0; i < N; i++) {
            assert(0 <= a[i] && a[i] < inf);
        }
        log = std::max(1, bit_ceil_log(inf));
        B.resize(1 << log);

        std::vector<std::tuple<int, int, int>> A(N); // {ブロック, 値, 元のインデックス}
        for (int i = 0; i < N; i++) A[i] = {1, a[i], i};

        f(A);
        for (int b = log - 1; b >= 0; b--) {
            std::vector<std::tuple<int, int, int>> ALLB;
            int l = 0;
            while (l < N) {
                std::vector<std::tuple<int, int, int>> LB, RB;
                int block = std::get<0>(A[l]);
                int r = l + 1;
                while (r < N && std::get<0>(A[r]) == block) {
                    r++;
                }
                std::vector<bool> bits(r - l);
                for (int i = l; i < r; i++) {
                    auto [bi, ai, idx] = A[i];
                    if (((ai >> b) & 1) == 0) {
                        LB.push_back({bi * 2, ai, idx});
                        bits[i - l] = 0;
                    } else {
                        RB.push_back({bi * 2 + 1, ai, idx});
                        bits[i - l] = 1;
                    }
                }
                f(LB);
                f(RB);
                B[block] = dynamic_bitvector(bits);
                ALLB.insert(ALLB.end(), LB.begin(), LB.end());
                ALLB.insert(ALLB.end(), RB.begin(), RB.end());
                l = r;
            }
            std::swap(ALLB, A);
        }
    }

    int size() {
        return B[1].size();
    }

    int access(int k) {
        int ans = 0, block = 1;
        for (int b = log - 1; b >= 0; b--) {
            int ro = B[block].rank(k);
            int rz = k - ro;
            if (B[block].get(k) == 0) {
                k = rz;
                block = block * 2;
            } else {
                k = ro;
                ans += 1 << b;
                block = block * 2 + 1;
            }
        }
        return ans;
    }

    // (0 <= 高さ <= log)の各高さで f(ブロック, インデックス)を呼び出す 
    template<typename F>
    int access_query(int k, F f) {
        int ans = 0, block = 1;
        for (int b = log - 1; b >= 0; b--) {
            f(block, k);
            int ro = B[block].rank(k);
            int rz = k - ro;
            if (B[block].get(k) == 0) {
                k = rz;
                block = block * 2;
            } else {
                k = ro;
                ans += 1 << b;
                block = block * 2 + 1;
            }
        }
        f(block, k);
        return ans;
    }

    // [0, r)のxの数
    int rank(int r, int x) {
        int block = 1;
        for (int b = log - 1; b >= 0; b--) {
            int ro = B[block].rank(r);
            int rz = r - ro;
            if (((x >> b) & 1) == 0) {
                r = rz;
                block = block * 2;
            } else {
                r = ro;
                block = block * 2 + 1;
            }
        }
        return r;
    }

    // k番目のxの位置 存在しない場合は-1
    int select(int k, int x) {
        int block = (x + (1 << log)) >> 1;
        for (int b = 0; b < log; b++, block >>= 1) {
            bool f = (x >> b) & 1;
            k = (f ? B[block].select(k) : B[block].select_zero(k));
            if (k == -1) return -1;
        }
        return k;
    }

    // k番目のxの位置を返して消す 存在しないと壊れる
    int select_erase(int k, int x) {
        int block = (x + (1 << log)) >> 1;
        for (int b = 0; b < log; b++, block >>= 1) {
            bool f = (x >> b) & 1;
            k = (f ? B[block].select(k) : B[block].select_zero(k));
            assert(k != -1);
            B[block].erase(k);
        }
        return k;
    }

    void insert(int k, int x) {
        int block = 1;
        for (int b = log - 1; b >= 0; b--) {
            int ro = B[block].rank(k);
            int rz = k - ro;
            bool f = (x >> b) & 1;
            B[block].insert(k, f);
            if (!f) {
                block = block * 2;
                k = rz;
            } else {
                block = block * 2 + 1;
                k = ro;
            }
        }
    }

    int erase(int k) {
        int block = 1, ans = 0;
        for (int b = log - 1; b >= 0; b--) {
            int ro = B[block].rank(k);
            int rz = k - ro;
            bool f = B[block].erase(k);
            if (!f) {
                block = block * 2;
                k = rz;
            } else {
                ans += 1 << b;
                block = block * 2 + 1;
                k = ro;
            }
        }
        return ans;
    }

    // [l, r)に現れるs以上t未満の要素数
    int range_freq(int _l, int _r, int s, int t) {
        auto dfs = [&](auto &&dfs, int block, int l, int r, int S, int T) -> int {
            if (l == r || T <= s || t <= S) return 0;
            if (s <= S && T <= t) return r - l;
            int lo = B[block].rank(l);
            int lz = l - lo;
            int ro = B[block].rank(r);
            int rz = r - ro;
            return dfs(dfs, block * 2, lz, rz, S, (S + T) / 2) + dfs(dfs, block * 2 + 1, lo, ro, (S + T) / 2, T);
        };
        if (_l >= _r || s >= t) return 0;
        return dfs(dfs, 1, _l, _r, 0, 1 << log);
    }

    // [l, r)に現れるs以上t未満の要素数
    template<typename F>
    int range_freq_query(int _l, int _r, int s, int t, F f) {
        auto dfs = [&](auto &&dfs, int block, int l, int r, int S, int T) -> int {
            if (l == r || T <= s || t <= S) return 0;
            if (s <= S && T <= t) {
                f(block, l, r);
                return r - l;
            }
            int lo = B[block].rank(l);
            int lz = l - lo;
            int ro = B[block].rank(r);
            int rz = r - ro;
            return dfs(dfs, block * 2, lz, rz, S, (S + T) / 2) + dfs(dfs, block * 2 + 1, lo, ro, (S + T) / 2, T);
        };
        if (_l >= _r || s >= t) return 0;
        return dfs(dfs, 1, _l, _r, 0, 1 << log);
    }

    // [l, r)のk番目に小さい数
    int kth_smallest(int l, int r, int k) {
        assert(l < r);
        assert(0 <= k && k < r - l);
        int ans = 0, block = 1;
        for (int b = log - 1; b >= 0; b--) {
            int lo = B[block].rank(l);
            int lz = l - lo;
            int ro = B[block].rank(r);
            int rz = r - ro;
            int zero = rz - lz;
            if (k < zero) {
                l = lz;
                r = rz;
                block = block * 2;
            } else {
                k -= zero;
                l = lo;
                r = ro;
                ans += 1 << b;
                block = block * 2 + 1;

            }
        }
        return ans;
    }
    
    // [l, r)のk番目に大きい数
    int kth_largest(int l, int r, int k) {
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

    // res[k] := [l, r)かつxとlcpがちょうどkである要素の個数
    std::vector<int> lcp_freq(int l, int r, int x) {
        int block = 1;
        std::vector<int> res(log + 1, 0);
        for (int b = log - 1; b >= 0; b--) {
            int lo = B[block].rank(l);
            int lz = l - lo;
            int ro = B[block].rank(r);
            int rz = r - ro;
            int len = r - l;
            if (((x >> b) & 1) == 0) {
                l = lz;
                r = rz;
                block = block * 2;
            } else {
                l = lo;
                r = ro;
                block = block * 2 + 1;
            }
            res[log - 1 - b] = len - (r - l);
        }
        res[log] = r - l;
        return res;
    }

    std::vector<int> to_list() {
        int N = (B.size() < 1 ? 0 : B[1].size());
        std::vector<int> pos(N), res(N);
        std::iota(pos.begin(), pos.end(), 0);
        std::queue<std::tuple<int, int, int>> que;
        que.push({1, 0, N});
        while (!que.empty()) {
            auto [b, l, r] = que.front();
            que.pop();
            if (b >= (1 << log)) {
                for (int i = l; i < r; i++) {
                    res[pos[i]] = b - (1 << log);
                }
            } else {
                auto bit = B[b].to_list();
                std::vector<int> R;
                int j = l;
                for (int i = l; i < r; i++) {
                    if (!bit[i - l]) pos[j++] = pos[i];
                    else R.push_back(pos[i]);
                }
                que.push({b * 2, l, j});
                que.push({b * 2 + 1, j, r});
                for (int x : R) pos[j++] = x;
            }
        }
        return res;
    }
};
#endif