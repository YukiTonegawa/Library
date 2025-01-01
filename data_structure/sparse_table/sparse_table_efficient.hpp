#ifndef _SPARSE_TABLE_EFFICIENT_H_
#define _SPARSE_TABLE_EFFICIENT_H_

#include "sparse_table.hpp"

// 隣接B要素ごとに分ける
// 1. サイズN/Bのsparse_tableを作る O(N/B log(N/B))
// 2. サイズBのsparse_tableをN/B個作る O(N/B * Blog(B))
template<typename T, T (*op)(T, T), T (*e)()>
struct sparse_table_efficient {
  private:
    static constexpr int block_size = 8;
    static constexpr int block_size_log = 3;
    sparse_table<T, op, e> st, st_short;

  public:
    int N;
    sparse_table_efficient(): N(0) {}
    sparse_table_efficient(const std::vector<T> &v): N(v.size()) {
        if (v.empty()) return;
        int M = (N + block_size - 1) / block_size;
        std::vector<T> v2(M, e());
        for (int i = 0; i < M; i++) {
            v2[i] = e();
            int l = i * block_size, r = std::min(N, l + block_size);
            for (int j = l; j < r; j++) {
                v2[i] = op(v2[i], v[j]);
            }
        }
        st = sparse_table<T, op, e>(v2);
        st_short = sparse_table<T, op, e>(v, block_size_log);
    }

    T prod(int l, int r) const {
        l = std::max(l, 0);
        r = std::min(r, N);
        if (l >= r) return e();
        int lb = (l + block_size - 1) / block_size, rb = r / block_size;
        int lceil = lb * block_size, rfloor = rb * block_size;
        T res = st.prod(lb, rb);
        res = op(res, st_short.prod(l, std::min(r, lceil)));
        res = op(res, st_short.prod(std::max(l, rfloor), r));
        return res;
    }
};

template<typename T>
T _min_func(T a, T b){ return std::min(a, b); }

template<typename T>
using sparse_table_rmq = sparse_table_efficient<T, _min_func<T>, std::numeric_limits<T>::max>;
#endif