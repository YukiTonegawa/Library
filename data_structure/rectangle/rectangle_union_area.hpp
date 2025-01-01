#ifndef _RECT_UNION_AREA_H_
#define _RECT_UNION_AREA_H_
#include <array>
#include "../segtree/segtree_lazy_fast.hpp"

struct monoid_rectangle_union_area {
    using T = std::pair<int, int>;
    using Lazy = int;
    static constexpr int inf_half = std::numeric_limits<int>::max() / 2;
    static T id() { return {inf_half, 0}; }
    static Lazy id_lazy() { return 0; }
    static T op(T a, T b) {
        if (a.first != b.first) return a.first < b.first ? a : b;
        return {a.first, a.second + b.second};
    }
    static T apply(Lazy lz, T a, int sz) { return {a.first + lz, a.second}; }
    static Lazy propagate(Lazy a, Lazy b) { return a + b; }
};

// v[i] = lx, rx, ly, ry
long long rectangle_union_area(std::vector<std::array<int, 4>> v) {
    if (v.empty()) return 0;
    int n = v.size();
    struct event {
        int x, ly, ry;
        bool is_add;
    };

    std::vector<event> e;
    std::vector<std::pair<int, int>> y;
    for (int i = 0; i < n; i++) {
        auto [l, r, d, u] = v[i];
        y.push_back({d, i});
        y.push_back({u, i + n});
    }

    std::sort(y.begin(), y.end());
    // y_dist[i] := y[i + 1] - y[i]
    std::vector<std::pair<int, int>> y_dist;
    int y_unique = 0, prev_y = y[0].first;
    for (int i = 0; i < 2 * n; i++) {
        if (y[i].first != prev_y) {
            y_dist.push_back({0, y[i].first - prev_y});
            prev_y = y[i].first;
            y_unique++;
        }
        if (y[i].second < n) v[y[i].second][2] = y_unique;
        else v[y[i].second - n][3] = y_unique;
    }
    for (auto [l, r, d, u] : v) {
        e.push_back(event{l, d, u, true});
        e.push_back(event{r, d, u, false});
    }
    std::sort(e.begin(), e.end(), [](event &a, event &b) { return a.x < b.x; });
    int y_all = y.back().first - y[0].first;

    using M = monoid_rectangle_union_area;
    segtree_lazy_fast<M::T, M::op, M::id, M::Lazy, M::apply, M::propagate, M::id_lazy> seg(y_dist);
    long long ans = 0;
    for (int i = 0, cur_x = 0; i < 2 * n; i++) {
        auto [x, ly, ry, is_add] = e[i];
        auto [m, mcnt] = seg.all_prod();
        int dx = x - cur_x;
        if (m == 0) ans += (long long)(y_all - mcnt) * dx;
        else ans += (long long)y_all * dx;
        cur_x = x;
        if (is_add) seg.apply(ly, ry, 1);
        else seg.apply(ly, ry, -1);
    }
    return ans;
}
#endif