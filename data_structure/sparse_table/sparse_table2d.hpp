#ifndef _SPARSE_TABLE2D_H_
#define _SPARSE_TABLE2D_H_
#include <vector>
#include <cassert>
#include <algorithm>
#include <limits>
#include "sparse_table.hpp"

template<typename T, T (*op)(T, T), T (*e)()>
struct sparse_table2d {
    int msb(int x) {
        return (x == 1 ? 0 : 31 - __builtin_clz(x - 1));
    }
  private:
    std::vector<std::vector<std::vector<T>>> table;
    int N, M, M2;
    sparse_table2d(): N(0), M(0) {}
    sparse_table2d(const std::vector<std::vector<T>> &v) : N(v.size()), M(0) {
        if (N == 0) return;
        int N2 = 0;
        M = v[0].size(), M2 = 0;
        while((1 << N2) <= N) N2++;
        while((1 << M2) <= M) M2++;
        table.resize(N2 * M2, std::vector<std::vector<T>>(N, std::vector<T>(M)));
        table[0] = v;

        // y
        for (int i = 0; i < N2; i++) {
            for (int j = 1; j < M2; j++) {
                int yshift = 1 << (j - 1);
                int zex = i * M2 + j;
                for (int x = 0; x < N; x++) {
                    for (int y = 0; y + yshift < M; y++) {
                        table[zex][x][y] = op(table[zex - 1][x][y], table[zex - 1][x][y + yshift]);
                    }
                }
            }
        }

        // x
        for (int j = 0; j < M2; j++) {
            for (int i = 1; i < N2; i++) {
                int xshift = 1 << (i - 1);
                int zex = i * M2 + j;
                for (int x = 0; x + xshift < N; x++) {
                    for (int y = 0; y < M; y++) {
                        table[zex][x][y] = op(table[zex - M2][x][y], table[zex - M2][x + xshift][y]);
                    }
                }
            }
        }
    }

    T prod(int lx, int rx, int ly, int ry) {
        lx = std::max(lx, 0);
        rx = std::min(rx, N);
        ly = std::max(ly, 0);
        ry = std::min(ry, M);
        if (lx >= rx || ly >= ry) return e();
        int bx = msb(rx - lx);
        int by = msb(ry - ly);
        int bz = bx * M2 + by;
        T lower = op(table[bz][lx][ly], table[bz][lx][ry - (1 << by)]);
        T upper = op(table[bz][rx - (1 << bx)][ly], table[bz][rx - (1 << bx)][ry - (1 << by)]);
        return op(lower, upper);
    }
};

#endif