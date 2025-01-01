#ifndef _POLYNOMIAL_INTERPOLATION_H_
#define _POLYNOMIAL_INTERPOLATION_H_
#include "fps.hpp"
#include "../base/bit_ceil.hpp"

template<typename FPS>
FPS polynomial_interpolation(const std::vector<typename FPS::mint_t> &x, const std::vector<typename FPS::mint_t> &y) {
    if (x.empty()) return {};
    int N = x.size(), M = bit_ceil(N);
    std::vector<FPS> t(2 * M - 1, FPS{});
    for (int i = 0; i < N; i++) t[M - 1 + i] = {-x[i], 1};
    for (int i = M - 2; i >= 0; i--) {
        if (t[i * 2 + 2].empty()) t[i] = t[i * 2 + 1];
        else t[i] = t[i * 2 + 1] * t[i * 2 + 2];
    }
    for (int i = 0; i < N; i++) t[0][i] = t[0][i + 1] * (i + 1);
    t[0].pop_back();

    auto dfs = [&](auto &&dfs, FPS &f, int k, int l, int r) -> FPS {
        if (r - l == 1) return {y[l] * f[0].inv()};
        int m = (l + r) / 2;
        if (t[k * 2 + 2].empty()) return dfs(dfs, f, k * 2 + 1, l, m);
        auto L = dfs(dfs, f % t[k * 2 + 1], k * 2 + 1, l, m);
        auto R = dfs(dfs, f % t[k * 2 + 2], k * 2 + 2, m, r);
        L *= t[k * 2 + 2];
        R *= t[k * 2 + 1];
        L.resize(std::max(L.size(), R.size()), 0);
        for (int i = 0; i < R.size(); i++) L[i] += R[i];
        return L;
    };
    auto ans = dfs(dfs, t[0], 0, 0, M);
    ans.resize(N);
    return ans;
}
#endif