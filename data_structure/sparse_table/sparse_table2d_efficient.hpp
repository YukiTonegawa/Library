#ifndef _SPARSE_TABLE2D_EFFICIENT_H_
#define _SPARSE_TABLE2D_EFFICIENT_H_
#include <vector>
#include <cassert>
#include <algorithm>
#include <limits>
#include "sparse_table_efficient.hpp"
#include "sparse_table2d.hpp"

// 前処理 O((NM/B^2) log(N/B)log(M/B)) + O(NM log(NM))
// クエリ O(1) + O(B)
template<typename T, T (*op)(T, T), T (*e)()>
struct sparse_table2d_efficient {
  private:
    static constexpr int block_size = 4;
    static constexpr int block_size_log = 2;
    sparse_table2d<T, op, e> st;
    std::vector<sparse_table_efficient<T, op, e>> row, col;

  public:
    int N, M;
    sparse_table2d_efficient() : N(0), M(0) {}
    
    sparse_table2d_efficient(const std::vector<std::vector<T>> &v) : N(v.size()), M(0) {
        if (v.empty()) return;
        M = v[0].size();
        int N2 = (N + block_size - 1) / block_size;
        int M2 = (M + block_size - 1) / block_size;
        std::vector<std::vector<T>> v2(N2, std::vector<T>(M2));

        for (int i = 0; i < N2; i++) {
            for (int j = 0; j < M2; j++) {
                v2[i][j] = e();
                int lx = i * block_size;
                int rx = std::min(N, lx + block_size);
                int ly = j * block_size;
                int ry = std::min(M, ly + block_size);
                for (int x = lx; x < rx; x++) {
                    for (int y = ly; y < ry; y++) {
                        v2[i][j] = min_func(v2[i][j], v[x][y]);
                    }
                }
            }
        }
        st = sparse_table2d<T, op, e>(v2);
        row.resize(N), col.resize(M);

        for (int i = 0; i < N; i++) {
            row[i] = sparse_table_efficient<T, op, e>(v[i]);
        }

        v2.resize(M, std::vector<T>(N));
        for (int i = 0; i < N; i++) {
            for(int j = 0; j < M; j++) {
                v2[j][i] = v[i][j];
            }
        }

        for (int i = 0; i < M; i++) {
            col[i] = sparse_table_efficient<T, op, e>(v2[i]);
        }
    }

    T prod(int lx, int rx, int ly, int ry) {
        lx = std::max(lx, 0);
        rx = std::min(rx, N);
        ly = std::max(ly, 0);
        ry = std::min(ry, M);
        if (lx >= rx || ly >= ry) return e();
    
        int lxb = (lx + block_size - 1) / block_size, rxb = rx / block_size;
        int lyb = (ly + block_size - 1) / block_size, ryb = ry / block_size;

        T res = st.prod(lxb, rxb, lyb, ryb);

        for (int i = lx; i < lxb * block_size; i++) {
            res = op(res, row[i].prod(ly, ry));
        }

        for (int i = rxb * block_size; i < rx; i++) {
            res = op(res, row[i].prod(ly, ry));
        }

        for (int i = ly; i < lyb * block_size; i++) {
            res = op(res, col[i].prod(lx, rx));
        }

        for (int i = ryb * block_size; i < ry; i++) {
            res = op(res, col[i].prod(lx, rx));
        }

        return res;
    }
};

template<typename T>
using sparse_table2d_rmq = sparse_table2d_efficient<T, _min_func<T>, std::numeric_limits<T>::max>;
#endif
