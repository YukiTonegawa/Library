#ifndef _RAMINMAX_FREQ_H_
#define _RAMINMAX_FREQ_H_
#include <vector>
#include <algorithm>
#include <numeric>
#include <cassert>
#include "../../function/clamp_function.hpp"

template<typename T>
struct range_add_chmin_chmax_range_freq {
  private:
    static constexpr T inf = std::numeric_limits<T>::max();
    static constexpr T minf = std::numeric_limits<T>::min();
    using F = clamp_function<T>;
    int N;
    std::vector<T> V, B;
    std::vector<F> lz;
    std::vector<bool> is_sorted;
    static constexpr int sq = 256;
  
  public:
    range_add_chmin_chmax_range_freq(int _N) : range_add_chmin_chmax_range_freq(std::vector<T>(_N, 0)) {}
    range_add_chmin_chmax_range_freq(const std::vector<T> &_V) : N(_V.size()), V(_V), lz((N + sq - 1) / sq, F()), is_sorted((N + sq - 1) / sq, true) {
        B = V;
        for (int i = 0; i < (N + sq - 1) / sq; i++) {
            int L = i * sq;
            int R = std::min(L + sq, N);
            std::sort(B.begin() + L, B.begin() + R);
        }
    }

    // [l, r)の x <- min(max(x + add, lower), upper)
    void apply(int l, int r, T add, T lower, T upper) {
        assert(l <= r && lower <= upper);
        int lb = (l + sq - 1) / sq, rb = r / sq;
        F f{add, lower, upper};
        if (l < lb * sq) is_sorted[lb - 1] = false;
        if (rb * sq < r) is_sorted[rb] = false;
        if (lb > rb) {
            int L = rb * sq, R = std::min(N, L + sq);
            for (int i = L; i < R; i++) {
                V[i] = lz[rb].fx(V[i]);
                if (l <= i && i < r) V[i] = f.fx(V[i]);
            }
            lz[rb] = F{};
            return;
        }
        if (l < lb * sq) {
            int L = (lb - 1) * sq, R = std::min(N, L + sq);
            for (int i = L; i < l; i++) {
                V[i] = lz[lb - 1].fx(V[i]);
            }
            for (int i = l; i < R; i++) {
                V[i] = f.fx(lz[lb - 1].fx(V[i]));
            }
            lz[lb - 1] = F{};
        }
        if (rb * sq < r) {
            int L = rb * sq, R = std::min(N, L + sq);
            for (int i = L; i < r; i++) {
                V[i] = f.fx(lz[rb].fx(V[i]));
            }
            for (int i = r; i < R; i++) {
                V[i] = lz[rb].fx(V[i]);
            }
            lz[rb] = F{};
        }
        for (int i = lb; i < rb; i++) {
            lz[i] = F::merge(lz[i], f);
        }
    }

    // [l, r)のx以上の最小要素
    T lower_bound(int l, int r, T x) {
        assert(l <= r);
        int lb = (l + sq - 1) / sq, rb = r / sq;
        T res = inf;
        if (lb > rb) {
            for (int i = l; i < r; i++) {
                T y = lz[rb].fx(V[i]);
                if (y >= x) res = std::min(res, y);
            }
            return res;
        }
        for (int i = l; i < lb * sq; i++) {
            T y = lz[lb - 1].fx(V[i]);
            if (y >= x) res = std::min(res, y);
        }
        for (int i = rb * sq; i < r; i++) {
            T y = lz[rb].fx(V[i]);
            if (y >= x) res = std::min(res, y);
        }
        for (int i = lb; i < rb; i++) {
            if (lz[i].upper < x) continue;
            int L = i * sq, R = std::min(N, L + sq);
            if (!is_sorted[i]) {
                for (int j = L; j < R; j++) B[j] = V[j];
                std::sort(B.begin() + L, B.begin() + R);
                is_sorted[i] = true;
            }
            auto itr = std::lower_bound(B.begin() + L, B.begin() + R, x - lz[i].add);
            if (lz[i].lower >= x && itr != B.begin() + L) {
                res = std::min(res, lz[i].lower);
            }
            if (itr != B.begin() + R) {
                res = std::min(res, lz[i].fx(*itr));
            }
        }
        return res;
    }

    // [l, r)のrx未満の要素数
    int range_freq(int l, int r, T rx) {
        assert(l <= r);
        int lb = (l + sq - 1) / sq, rb = r / sq;
        int res = 0;
        if (lb > rb) {
            for (int i = l; i < r; i++) {
                T y = lz[rb].fx(V[i]);
                if (y < rx) res++;
            }
            return res;
        }
        for (int i = l; i < lb * sq; i++) {
            T y = lz[lb - 1].fx(V[i]);
            if (y < rx) res++;
        }
        for (int i = rb * sq; i < r; i++) {
            T y = lz[rb].fx(V[i]);
            if (y < rx) res++;
        }
        for (int i = lb; i < rb; i++) {
            if (lz[i].lower >= rx) continue;
            int L = i * sq, R = std::min(N, L + sq);
            if (lz[i].upper < rx) {
                res += R - L;
                continue;
            }
            if (!is_sorted[i]) {
                for (int j = L; j < R; j++) B[j] = V[j];
                std::sort(B.begin() + L, B.begin() + R);
                is_sorted[i] = true;
            }
            res += std::lower_bound(B.begin() + L, B.begin() + R, rx - lz[i].add) - (B.begin() + L);
        }
        return res;
    }
};
#endif