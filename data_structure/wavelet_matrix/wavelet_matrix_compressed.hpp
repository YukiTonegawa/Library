#ifndef _WAVELET_MATRIX_COMPRESSED_H_
#define _WAVELET_MATRIX_COMPRESSED_H_
#include <vector>
#include "wavelet_matrix.hpp"
#include "../../base/compressor.hpp"

template<typename T>
struct wavelet_matrix_compressed {
  private:
    std::vector<T> rev;
    compressor<T> cmp;

    wavelet_matrix wm;
    
  public:
    wavelet_matrix_compressed() {}
    wavelet_matrix_compressed(const std::vector<T> &_v) {
        cmp.add(_v.begin(), _v.end());
        int N = _v.size();
        std::vector<int> v(N);
        for (int i = 0; i < N; i++) v[i] = cmp.ord(_v[i]);
        wm = wavelet_matrix(v);
    }

    // build時に(0 <= 高さ <= log)の各高さでf(元のインデックスのvector)を呼び出す
    template<typename F>
    wavelet_matrix_compressed(const std::vector<T> &_v, F f) {
        cmp.add(_v.begin(), _v.end());
        int N = _v.size();
        std::vector<int> v(N);
        for (int i = 0; i < N; i++) v[i] = cmp.ord(_v[i]);
        wm = wavelet_matrix(v, f);
    }

    // V[k]
    T access(int k) {
        return cmp[wm.access(k)];
    }

    // (0 <= 高さ <= log)の各高さで f(高さ, インデックス)を呼び出す 
    template<typename F>
    T access_query(int k, F f) {
        return cmp[wm.access_query<F>(k, f)];
    }

    // [0, r)のxの数
    int rank(int r, T x) {
        int ord = cmp.ord(x);
        if (ord == cmp.size() || cmp[ord] != x) return 0;
        return wm.rank(r, ord);
    }

    // k番目のxの位置 存在しない場合は-1
    int select(int k, T x) {
        int ord = cmp.ord(x);
        if (ord == cmp.size() || cmp[ord] != x) return -1;
        return wm.select(k, ord);
    }

    // [l, r)に現れるs以上t未満の要素数
    int range_freq(int l, int r, T s, T t) {
        int _s = cmp.ord(s);
        int _t = cmp.ord(t);
        return wm.range_freq(l, r, _s, _t);
    }

    // O(logN)個の区間に対して f(高さ, l, r)を呼び出す (0 <= 高さ <= log)
    template<typename F>
    int range_freq_query(int l, int r, T s, T t, F f) {
        int _s = cmp.ord(s);
        int _t = cmp.ord(t);
        return wm.range_freq_query<F>(l, r, _s, _t, f);
    }

    // [l, r)のk番目に小さい数
    T kth_smallest(int l, int r, int k) {
        return cmp[wm.kth_smallest(l, r, k)];
    }
    
    // [l, r)のk番目に大きい数
    T kth_largest(int l, int r, int k) {
        return cmp[kth_largest(l, r, k)];
    }

    // [l, r)のx以上の最小要素が存在するか, 存在する場合その値
    std::pair<bool, T> ge(int l, int r, T x) {
        int ord = cmp.ord(x);
        int res = wm.ge(l, r, ord);
        if (res == -1) return {false, 0};
        return {true, cmp[res]};
    }

    // [l, r)のxより大きい最小要素が存在するか, 存在する場合その値
    std::pair<bool, T> gt(int l, int r, T x) {
        return ge(l, r, x + 1);
    }

    // [l, r)のx以下の最大要素が存在するか, 存在する場合その値
    std::pair<bool, T> le(int l, int r, T x) {
        return lt(l, r, x + 1);
    }

    // [l, r)のx未満の最大要素が存在するか, 存在する場合その値
    std::pair<bool, T> lt(int l, int r, T x) {
        int ord = cmp.ord(x);
        int res = wm.lt(l, r, ord);
        if (res == -1) return {false, 0};
        return {true, cmp[res]};
    }

    // [l, N)で最も左にあるxの位置 存在しない場合は-1
    int next_pos(int l, T x) {
        int ord = cmp.ord(x);
        if (ord == cmp.size() || cmp[ord] != x) return -1;
        return wm.next_pos(l, ord);
    }

    // [0, r)で最も右にあるxの位置 存在しない場合は-1
    int prev_pos(int r, T x) {
        int ord = cmp.ord(x);
        if (ord == cmp.size() || cmp[ord] != x) return -1;
        return wm.prev_pos(r, ord);
    }
};
#endif
