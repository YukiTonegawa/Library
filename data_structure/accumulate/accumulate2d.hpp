#ifndef _ACCUMULATE2d_H_
#define _ACCUMULATE2d_H_
#include <vector>
#include <iostream>
#include <functional>
#include <cassert>

template<typename Val>
struct accumulate2d {
    int N, M;
    std::vector<std::vector<Val>> sum;
    
    accumulate2d(){}
    accumulate2d(const std::vector<std::vector<Val>> &v) {
        N = v.size();
        M = (!N ? 0 : v[0].size());
        sum.resize(N + 1, std::vector<Val>(M + 1, 0));
        
        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= M; j++) {
                sum[i][j] = sum[i][j - 1] + v[i - 1][j - 1];
            }
        }

        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= M; j++) {
                sum[i][j] += sum[i - 1][j];
            }
        }
    }

    // [lx, rx) × [ly, ry)の和, 範囲外は全て単位元とする
    Val query(int lx, int rx, int ly, int ry) {
        lx = std::max(lx, 0);
        ly = std::max(ly, 0);
        rx = std::min(rx, N);
        ry = std::min(ry, M);
        if (lx >= rx || ly >= ry) return 0;
        return sum[rx][ry] - sum[rx][ly] - sum[lx][ry] + sum[lx][ly];
    }
};
#endif