#ifndef _ACCUMULATE_3D_H_
#define _ACCUMULATE_3D_H_

#include <array>

template<typename Val>
struct accumulate3d {
    int N, M, K;
    std::vector<std::vector<std::vector<Val>>> sum;
    
    accumulate3d(){}
    accumulate3d(const std::vector<std::vector<std::vector<Val>>> &v) {
        N = v.size();
        M = (!N ? 0 : v[0].size());
        K = (!M ? 0 : v[0][0].size());
        
        sum.resize(N + 1, std::vector<std::vector<Val>>(M + 1, std::vector<Val>(K + 1)));
        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= M; j++) {
                for (int k = 1; k <= K; k++) {
                    sum[i][j][k] = sum[i][j][k - 1] + v[i - 1][j - 1][k - 1];
                }
            }
        }

        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= N; j++) {
                for (int k = 1; k <= K; k++) {
                    sum[i][j][k] += sum[i][j - 1][k];
                }
            }
        }

        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= N; j++) {
                for (int k = 1; k <= K; k++) {
                    sum[i][j][k] += sum[i - 1][j][k];
                }
            }
        }
    }

    // [lx, rx, lz) × [ly, ry, rz)の和, 範囲外は全て単位元とする
    Val query(int lx, int rx, int ly, int ry, int lz, int rz) {
        lx = std::max(lx, 0);
        ly = std::max(ly, 0);
        lz = std::max(lz, 0);
        rx = std::min(rx, N);
        ry = std::min(ry, M);
        rz = std::min(rz, K);
        if (lx >= rx || ly >= ry || lz >= rz) return 0;

        Val ans = 0;
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    Val v = sum[i ? rx : lx][j ? ry : ly][k ? rz : lz];
                    if (i ^ j ^ k) {
                        ans += v;
                    } else {
                        ans -= v;
                    }
                }
            }
        }
        return ans;
    }
};
#endif