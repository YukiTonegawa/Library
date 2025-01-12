#ifndef _DISJOINT_SPARSE_TABLE_H_
#define _DISJOINT_SPARSE_TABLE_H_
#include <vector>
#include "../../math/base/bit_ceil.hpp"
template<typename S, S (*op)(S, S), S (*e)()>
struct disjoint_sparse_table {
  private:
    int log, size;
    std::vector<std::vector<S>> sum;

    static int msb(int i) {
        return 31 - __builtin_clz(i);
    }

  public:
    disjoint_sparse_table(int N = 0) : disjoint_sparse_table(std::vector<S>(N, e())) {}
    disjoint_sparse_table(std::vector<S> V) : log(bit_ceil_log(std::max(2, (int)V.size()))), size(1 << log), sum(log) {
        V.resize(size, e());
        for (int i = 0; i < log; i++) sum[i] = V;
        for (int i = 1; i < size; i++) {
            int d = msb(i), len = 1 << (log - d), l = len * (i - (1 << d)), r = l + len, m = l + len / 2;
            for (int j = m - 2; j >= l; j--) {
                sum[d][j] = op(sum[d][j], sum[d][j + 1]);
            }
            for (int j = m + 1; j < r; j++) {
                sum[d][j] = op(sum[d][j - 1], sum[d][j]);
            }
        }
    }

    S prod(int l, int r) const {
        if (l >= r) return e();
        if (r - l == 1) return sum[log - 1][l];
        r--;
        int d = log - 1 - msb(l ^ r);
        return op(sum[d][l], sum[d][r]);
    }
};


// 隣接B要素ごとに分ける
// 1. サイズN/Bのsparse_tableを作る O(N/B log(N/B))
// 2. 端のサイズBの部分は愚直に処理
template<typename S, S (*op)(S, S), S (*e)()>
struct disjoint_sparse_table_efficient {
  private:
    static constexpr int B = 8;
    disjoint_sparse_table<S, op, e> st;
    std::vector<S> V;

  public:
    disjoint_sparse_table_efficient(int N = 0) : disjoint_sparse_table_efficient(std::vector<S>(N, e())) {}
    disjoint_sparse_table_efficient(const std::vector<S> &V) : V(V) {
        int N = V.size(), M = (N + B - 1) / B;
        std::vector<S> V2(M, e());
        for (int i = 0; i < M; i++) {
            V2[i] = e();
            int l = i * B, r = std::min(N, l + B);
            for (int j = l; j < r; j++) {
                V2[i] = op(V2[i], V[j]);
            }
        }
        st = disjoint_sparse_table<S, op, e>(V2);
    }

    S prod(int l, int r) const {
        if (l >= r) return e();
        int lb = (l + B - 1) / B, rb = r / B;
        if (lb >= rb) {
            S res = e();
            for (int i = l; i < r; i++) res = op(res, V[i]);
            return res;
        } else {
            int lceil = lb * B, rfloor = rb * B;
            S res = st.prod(lb, rb);
            for (int i = l; i < lceil; i++) res = op(V[i], res);
            for (int i = rfloor; i < r; i++) res = op(res, V[i]);
            return res;
        }
    }
};
#endif