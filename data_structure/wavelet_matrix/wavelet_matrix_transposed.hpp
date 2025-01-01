#ifndef _WAVELET_MATRIX_TRANSPOSED_H_
#define _WAVELET_MATRIX_TRANSPOSED_H_
#include <vector>
#include <numeric>
#include <algorithm>
#include "wavelet_matrix.hpp"
#include "../../util/compressor.hpp"

template<typename T>
struct wavelet_matrix_transposed {
  private:
    int N;
    wavelet_matrix wm;
    compressor<T, false> cmp;

  public:
    wavelet_matrix_transposed() : N(0) {}
    wavelet_matrix_transposed(const std::vector<T> &_v) : N(_v.size()) {
        cmp.add(_v.begin(), _v.end());
        std::vector<int> next(N), idx(N, -1);
        std::iota(next.begin(), next.end(), 0);
        for (int i = 0; i < N; i++) {
            int ord = cmp.ord(_v[i]);
            idx[next[ord]++] = i;
        }
        wm = wavelet_matrix(idx);
    }

    T access(int k) {
        int ord = wm.select(0, k);
        assert(k != -1);
        return cmp[ord];
    }

    // [0, r)のxの数
    int rank(int r, T x) {
        int _s = cmp.ord(x);
        int _t = cmp.ord(x + 1);
        if (_s == _t) return 0;
        return wm.range_freq(_s, _t, 0, r);
    }

    // k番目のxの位置 存在しない場合は-1
    int select(int k, T x) {
        int _s = cmp.ord(x);
        if (_s == cmp.size() || cmp[_s] != x) return -1; 
        return wm.access(_s + k);
    }

    // k番目の値がs以上t未満の数の位置 存在しない場合は-1
    int range_select(int k, T s, T t) {
        int _s = cmp.ord(s);
        int _t = cmp.ord(t);
        return wm.kth_smallest(_s, _t, k);
    }

    // [l, r)に現れるs以上t未満の要素数
    int range_freq(int l, int r, T s, T t) {
        int _s = cmp.ord(s);
        int _t = cmp.ord(t);
        return wm.range_freq(_s, _t, l, r);
    }

    // A_{l}, A_{l+1}, .... A_{r-1} がs以上t未満であるような最大のr
    int range_max_right(int l, T s, T t) {
        // t以上の最左
        int _t = cmp.ord(t);
        int upper = wm.ge(_t, N, l);
        if (upper == -1) upper = N;
        // s未満の最左
        int _s = cmp.ord(s);
        int lower = wm.ge(0, _s, l);
        if (lower == -1) lower = N;
        return std::min(upper, lower);
    }

    // A_{l}, A_{l+1}, .... A_{r-1} がs以上t未満であるような最小のl
    int range_min_left(int r, T s, T t) {
        // t以上の最左
        int _t = cmp.ord(t);
        int upper = wm.lt(_t, N, r);
        // s未満の最左
        int _s = cmp.ord(s);
        int lower = wm.lt(0, _s, r);
        return std::max(upper, lower) + 1;
    }
};

#endif