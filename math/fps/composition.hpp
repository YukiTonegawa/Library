#ifndef _COMPOSITION_H_
#define _COMPOSITION_H_
#include <vector>
#include <cmath>
#include <cassert>

// f(g(x)) mod x^D
// O(D^2)
template<typename FPS>
FPS composition(FPS f, FPS g, int D = -1) {
    int N = f.size();
    if (D == -1) D = N;
    if (f.size() > D) f.resize(D), N = f.size();
    if (g.size() > D) g.resize(D);
    int B = std::sqrt(N) + 1;
    std::vector<FPS> gpow(B + 1);
    gpow[0] = {1};
    for (int i = 1; i <= B; i++) {
        gpow[i] = gpow[i - 1] * g;
        if (gpow[i].size() > D) gpow[i].resize(D);
    }
    std::vector<FPS> fi(B, FPS(D, 0));
    for (int i = 0; i < B; i++) {
        for (int j = 0; j < B; j++) {
            int p = i * B + j;
            if (p >= N) break;
            for (int t = 0; t < gpow[j].size(); t++) {
                fi[i][t] += gpow[j][t] * f[p];
            }
        }
    }
    FPS res(D, 0), gb = {1};
    for (int i = 0; i < B; i++) {
        fi[i] *= gb;
        for (int j = 0; j < D; j++) res[j] += fi[i][j];
        gb *= gpow[B];
        if (gb.size() > D) gb.resize(D);
    }
    return res;
}

// f(g(x)) = g(f(x)) = x mod x^D を満たすg
// O(D^2)
/*
template<typename FPS>
FPS compositional_inverse(FPS f, int D = -1) {
}
*/
#endif