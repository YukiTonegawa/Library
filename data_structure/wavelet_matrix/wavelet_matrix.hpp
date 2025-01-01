#ifndef _WAVELET_MATRIX_H_
#define _WAVELET_MATRIX_H_
#include "../bit/simple_bitvector.hpp"
#include "../../math/base/bit_ceil.hpp"

struct wavelet_matrix {
  private:
    int N, log, inf;
    std::vector<simple_bitvector> B;
    std::vector<int> leaf_idx;

  public:
    wavelet_matrix() : N(0) {}
    wavelet_matrix(const std::vector<int> &a) : N(a.size()), leaf_idx(N) {
        inf = 0;
        for (int i = 0; i < N; i++) {
            assert(0 <= a[i]);
            inf = std::max(inf, a[i] + 1);
        }
        log = std::max(1, bit_ceil_log(inf));
        B.resize(log);
        std::vector<std::pair<int, int>> A(N);
        for (int i = 0; i < N; i++) A[i] = {a[i], i};
        for (int b = log - 1; b >= 0; b--) {
            std::vector<bool> bits(N);
            std::vector<std::pair<int, int>> left, right;
            for (int i = 0; i < N; i++) {
                bits[i] = (A[i].first >> b) & 1;
                if (!bits[i]) {
                    left.push_back(A[i]);
                } else {
                    right.push_back(A[i]);
                }
            }
            left.insert(left.end(), right.begin(), right.end());
            std::swap(left, A);
            B[b] = simple_bitvector(bits);
        }
        for (int i = 0; i < N; i++) leaf_idx[i] = A[i].second;
    }

    // build時に(0 <= 高さ <= log)の各高さでf(元のインデックスのvector)を呼び出す
    template<typename F>
    wavelet_matrix(const std::vector<int> &a, F f) : N(a.size()), leaf_idx(N) {
        inf = 0;
        for (int i = 0; i < N; i++) {
            assert(0 <= a[i]);
            inf = std::max(inf, a[i] + 1);
        }
        log = std::max(1, bit_ceil_log(inf));
        B.resize(log);
        std::vector<std::pair<int, int>> A(N);
        std::vector<int> idx(N);
        for (int i = 0; i < N; i++) {
            A[i] = {a[i], i};
            idx[i] = i;
        }
        f(log, idx);
        for (int b = log - 1; b >= 0; b--) {
            std::vector<bool> bits(N);
            std::vector<std::pair<int, int>> left, right;
            for (int i = 0; i < N; i++) {
                bits[i] = (A[i].first >> b) & 1;
                if (!bits[i]) {
                    left.push_back(A[i]);
                } else {
                    right.push_back(A[i]);
                }
            }
            left.insert(left.end(), right.begin(), right.end());
            std::swap(left, A);
            B[b] = simple_bitvector(bits);
            for (int i = 0; i < N; i++) idx[i] = A[i].second;
            f(b, idx);
        }
        for (int i = 0; i < N; i++) leaf_idx[i] = A[i].second;
    }


    int access(int k) {
        int l = 0, r = N, ans = 0;
        for (int b = log - 1; b >= 0; b--) {
            int lo = B[b].rank(l);
            int one = B[b].rank(r) - lo;
            int zero = (r - l) - one;
            int klo = B[b].rank(k) - lo;
            if (B[b].access(k) == 0) {
                int klz = k - l - klo;
                l = l - lo;
                k = l + klz;
                r = l + zero;
            } else {
                ans += 1 << b;
                l = N - (B[b].rank(N) - lo);
                k = l + klo;
                r = l + one;
            }
        }
        return ans;
    }

    // (0 <= 高さ <= log)の各高さで f(高さ, インデックス)を呼び出す 
    template<typename F>
    int access_query(int k, F f) {
        int l = 0, r = N, ans = 0;
        for (int b = log - 1; b >= 0; b--) {
            f(b + 1, k);
            int lo = B[b].rank(l);
            int one = B[b].rank(r) - lo;
            int zero = (r - l) - one;
            int klo = B[b].rank(k) - lo;
            if (B[b].access(k) == 0) {
                int klz = k - l - klo;
                l = l - lo;
                k = l + klz;
                r = l + zero;
            } else {
                ans += 1 << b;
                l = N - (B[b].rank(N) - lo);
                k = l + klo;
                r = l + one;
            }
        }
        f(0, k);
        return ans;
    }

    // [0, r)のxの数
    int rank(int r, int x) {
        int l = 0;
        for (int b = log - 1; b >= 0; b--) {
            int lo = B[b].rank(l);
            int one = B[b].rank(r) - lo;
            int zero = (r - l) - one;
            if (((x >> b) & 1) == 0) {
                l = l - lo;
                r = l + zero;
            } else {
                l = N - (B[b].rank(N) - lo);
                r = l + one;
            }
        }
        return r - l;
    }

    // k番目のxの位置 存在しない場合は-1
    int select(int k, int x) {
        int l = 0, r = N;
        for (int b = log - 1; b >= 0; b--) {
            int lo = B[b].rank(l);
            int one = B[b].rank(r) - lo;
            int zero = (r - l) - one;
            if (((x >> b) & 1) == 0) {
                l = l - lo;
                r = l + zero;
            } else {
                l = N - (B[b].rank(N) - lo);
                r = l + one;
            }
        }
        if (r - l <= k) return -1;
        return leaf_idx[l + k];
    }

    // [l, r)に現れるs以上t未満の要素数
    int range_freq(int _l, int _r, int s, int t) {
        auto dfs = [&](auto &&dfs, int b, int l, int r, int S, int T) -> int {
            if (l == r || T <= s || t <= S) return 0;
            if (s <= S && T <= t) return r - l;
            int lo = B[b].rank(l);
            int one = B[b].rank(r) - lo;
            int zero = (r - l) - one;
            int l2 = l - lo;
            int r2 = l2 + zero;
            int L2 = N - (B[b].rank(N) - lo);
            int R2 = L2 + one;
            return dfs(dfs, b - 1, l2, r2, S, (S + T) / 2) + dfs(dfs, b - 1, L2, R2, (S + T) / 2, T);
        };
        if (_l >= _r || s >= t) return 0;
        return dfs(dfs, log - 1, _l, _r, 0, 1 << log);
    }

    // O(logN)個の区間に対して f(高さ, l, r)を呼び出す (0 <= 高さ <= log)
    template<typename F>
    int range_freq_query(int _l, int _r, int s, int t, F &f) {
        auto dfs = [&](auto &&dfs, int b, int l, int r, int S, int T) -> int {
            if (l == r || T <= s || t <= S) return 0;
            if (s <= S && T <= t) {
                f(b + 1, l, r);
                return r - l;
            }
            int lo = B[b].rank(l);
            int one = B[b].rank(r) - lo;
            int zero = (r - l) - one;
            int l2 = l - lo;
            int r2 = l2 + zero;
            int L2 = N - (B[b].rank(N) - lo);
            int R2 = L2 + one;
            return dfs(dfs, b - 1, l2, r2, S, (S + T) / 2) + dfs(dfs, b - 1, L2, R2, (S + T) / 2, T);
        };
        if (_l >= _r || s >= t) return 0;
        return dfs(dfs, log - 1, _l, _r, 0, 1 << log);
    }

    // [l, r)のk番目に小さい数
    int kth_smallest(int l, int r, int k) {
        assert(l < r);
        assert(0 <= k && k < r - l);
        int ans = 0;
        for (int b = log - 1; b >= 0; b--) {
            int lo = B[b].rank(l);
            int one = B[b].rank(r) - lo;
            int zero = (r - l) - one;
            if (k < zero) {
                l = l - lo;
                r = l + zero;
            } else {
                ans += 1 << b;
                k -= zero;
                l = N - (B[b].rank(N) - lo);
                r = l + one;
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
        if (x < 0 || x >= inf) return -1;
        int cnt = rank(l, x);
        return select(cnt, x);
    }

    // [0, r)で最も右にあるxの位置 存在しない場合は-1
    int prev_pos(int r, int x) {
        if (x < 0 || x >= inf) return -1;
        int cnt = rank(r, x);
        if (cnt == 0) return -1;
        return select(cnt - 1, x);
    }

    // res[k] := [l, r)かつxとlcpがちょうどkである要素の個数
    std::vector<int> lcp_freq(int l, int r, int x) {
        std::vector<int> res(log + 1, 0);
        for (int b = log - 1; b >= 0; b--) {
            int lo = B[b].rank(l);
            int one = B[b].rank(r) - lo;
            int zero = (r - l) - one;
            int len = r - l;
            if (((x >> b) & 1) == 0) {
                l = l - lo;
                r = l + zero;
            } else {
                l = N - (B[b].rank(N) - lo);
                r = l + one;
            }
            res[log - 1 - b] = len - (r - l);
        }
        res[log] = r - l;
        return res;
    }
};
#endif