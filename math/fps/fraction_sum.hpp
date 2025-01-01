#ifndef _FRACTION_SUM_H_
#define _FRACTION_SUM_H_
#include "fps.hpp"

// V : {{a0, b0, c0}, {a1, b1, c1}, ...}
// a / (bx + c) を大量に足す
template<typename FPS>
FPS fraction_sum(const std::vector<std::tuple<typename FPS::mint_t, typename FPS::mint_t, typename FPS::mint_t>> &V) {
    int N = V.size();
    std::vector<FPS> Q, P;
    for (auto [a, b, c] : V) {
        assert(c.val());
        Q.push_back({a});
        P.push_back({c, b});
    }
    for (int d = 1; d < N; d *= 2) {
        for (int j = 0; j < N; j += 2 * d) {
            if (j + d < N) {
                Q[j] = Q[j] * P[j + d] + Q[j + d] * P[j];
                P[j] *= P[j + d];
            }
        }
    }
    return Q[0] * P[0].inv();
}
#endif