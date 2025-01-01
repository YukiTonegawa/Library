#ifndef _MIN_PLUS_CONVOLUTION_H_
#define _MIN_PLUS_CONVOLUTION_H_
#include <vector>
#include <limits>
#include <queue>


// Aが下に凸
template<typename T>
std::vector<T> min_plus_convolution(const std::vector<T> &a, const std::vector<T> &b) {
    int n = a.size(), m = b.size();
    std::vector<T> c(n + m - 1);

    auto calc_min = [&](int k, int lb, int rb) -> std::pair<int, T> {
        std::pair<int, T> mn{lb, std::numeric_limits<T>::max()};
        for (int i = lb; i < rb; i++) {
            if (k - i < 0 || k - i >= n) continue;
            if (a[k - i] + b[i] < mn.second) {
                mn = {i, a[k - i] + b[i]};
            }
        }
        return mn;
    };

    auto dfs = [&](auto &&dfs, int lc, int rc, int lb, int rb) -> void {
        int mc = (lc + rc) / 2;
        auto [mb, x] = calc_min(mc, lb, rb);
        c[mc] = x;
        if (lc < mc) dfs(dfs, lc, mc, lb, mb + 1);
        if (mc + 1 < rc) dfs(dfs, mc + 1, rc, mb, rb);
    };

    dfs(dfs, 0, n + m - 1, 0, m);
    return c;
}

// Aが上に凸
template<typename T>
std::vector<T> min_plus_convolution_concave(const std::vector<T> &a, const std::vector<T> &b) {
    static constexpr T inf = std::numeric_limits<T>::max();
    int n = a.size(), m = b.size();
    std::vector<T> c(n + m - 1, inf);

    auto calc_min = [&](int k, int lb, int rb) -> std::pair<int, T> {
        rb = std::min({rb, k + 1, m});
        lb = std::max({lb, k - (n - 1), 0});
        std::pair<int, T> mn{lb, inf};
        for (int i = lb; i < rb; i++) {
            if (k - i < 0 || k - i >= n) continue;
            if (a[k - i] + b[i] < mn.second) {
                mn = {i, a[k - i] + b[i]};
            }
        }
        return mn;
    };

    auto dfs = [&](auto &&dfs, int lc, int rc, int lb, int rb) -> void {
        lc = std::max(lc, lb);
        rc = std::min(rc, rb + n - 1);
        if (lc >= rc || lb >= rb) return;
        int mc = (lc + rc) / 2;
        auto [mb, x] = calc_min(mc, lb, rb);
        if (x == inf) return;
        c[mc] = std::min(c[mc], x);
        dfs(dfs, lc, mc, lb, std::min(mb + 1, mc - (n - 1)));
        dfs(dfs, lc, std::min(mb, mc), std::max(lb, mc - n), mb);
        dfs(dfs, std::max(mb, lc), mc, mb, rb);
        dfs(dfs, mc + 1, std::min(rc, mb + n), std::max(lb, mc - (n - 1)), mb + 1);
        dfs(dfs, std::max(mc + 1, mb + n - 1), rc, mb, std::min(rb, mc));
        dfs(dfs, mc + 1, rc, std::max(mc, lb), rb);
    };

    dfs(dfs, 0, n + m - 1, 0, m);
    return c;
}
#endif