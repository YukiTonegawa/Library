#ifndef _SPARSE_TABLE_H_
#define _SPARSE_TABLE_H_
#include <vector>
#include <cassert>
#include <algorithm>
#include <limits>

template<typename T, T (*op)(T, T), T (*e)()>
struct sparse_table {
  private:
    int N;
    std::vector<std::vector<T>> table;

  public:
    sparse_table() : N(0) {}
    // max_height := 長さ2^max_height以内のクエリしか来ないことがわかっている場合高さを制限できる
    sparse_table(const std::vector<T> &v, int max_height = -1) : N(v.size()) {
        if (v.empty()) return;
        int M = 0;
        while ((1 << M) <= N) M++;
        if (max_height != -1) M = std::min(M, max_height);
        table.resize(M);
        if (M <= 0) return;
        table[0] = v;
        for (int i = 1; i < M; i++) {
            int shift = 1 << (i - 1);
            assert(table[i - 1].size() >= shift);
            table[i].resize(table[i - 1].size() - shift);
            for (int j = 0; j < table[i].size(); j++) {
                table[i][j] = op(table[i - 1][j], table[i - 1][j + shift]);
            }
        }
    }

    T prod(int l, int r) const {
        l = std::max(l, 0);
        r = std::min(r, N);
        if (l >= r) return e();
        int msb = 31 - __builtin_clz(r - l);
        return op(table[msb][l], table[msb][r - (1 << msb)]);
    }
};

#endif
