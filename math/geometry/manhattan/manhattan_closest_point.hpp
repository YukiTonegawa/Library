#ifndef _MANHATTAN_CLOSEST_POINT_H_
#define _MANHATTAN_CLOSEST_POINT_H_
#include <tuple>
#include <algorithm>
#include <vector>
#include "../../../data_structure/fenwick_tree/fenwick_tree_prefix_min.hpp"

/*      y
          ┃
        7 ┃ 0
      6   ┃   1
  ━━━━━━━━━━━━━━━━━━  x
      5   ┃   2
       4  ┃ 3
          ┃

*/

template<typename T>
std::pair<T, int> _e() {
    return {std::numeric_limits<T>::max(), -1};
}

template<typename T>
std::vector<std::tuple<T, int, int>> manhattan_closest_point(const std::vector<std::pair<T, T>> &_P) {
    struct point {
        T x, y;
        int cx, cy, id; // 座標圧縮後のx, y
        point() {}
        point(T x, T y, int id): x(x), y(y), id(id) {}
    };

    std::vector<std::tuple<T, int, int>> E;
    int N = _P.size();

    std::vector<point> P(N);

    auto compress = [&]() { 
        std::vector<std::pair<T, int>> x(N), y(N);
        for (int i = 0; i < N; i++) {
            P[i] = point(_P[i].first, _P[i].second, i);
            x[i] = {P[i].x, i};
            y[i] = {P[i].y, i};
        }
        std::sort(x.begin(), x.end());
        std::sort(y.begin(), y.end());
        for (int i = 0; i < N; i++) {
            P[x[i].second].cx = i;
            P[y[i].second].cy = i;
        }
    };
    
    compress();

    fenwick_tree_prefix_min<std::pair<T, int>, _e<T>> S1(N), S2(N);

    
    std::sort(P.begin(), P.end(), [](const point &a, const point &b) { return a.x + a.y < b.x + b.y; });

    // x + yの昇順
    for (int i = 0; i < N; i++) {
        const point &p = P[i];
        // 3, x + yがpより小さく, xがp以上
        auto [dist, to] = S1.prod(N - 1 - p.cx);
        if (to != -1) E.push_back({dist - p.x + p.y, p.id, to});

        // 6, x + yがpより小さく, yがp以上
        std::tie(dist, to) = S2.prod(N - 1 - p.cy);
        if (to != -1) E.push_back({dist + p.x - p.y, p.id, to});

        S1.apply(N - 1 - p.cx, {p.x - p.y, p.id});
        S2.apply(N - 1 - p.cy, {-p.x + p.y, p.id});
    }

    S1.reset();
    S2.reset();
        
    // x + yの降順
    for (int i = N - 1; i >= 0; i--) {
        const point &p = P[i];
        // 7, x + yがpより大きく, xがp以下
        auto [dist, to] = S1.prod(p.cx);
        if (to != -1) E.push_back({dist + p.x - p.y, p.id, to});

        // 2, x + yがpより大きく, yがp以下
        std::tie(dist, to) = S2.prod(p.cy);
        if (to != -1) E.push_back({dist - p.x + p.y, p.id, to});

        S1.apply(p.cx, {-p.x + p.y, p.id});
        S2.apply(p.cy, {p.x - p.y, p.id});
    }
    
    std::sort(P.begin(), P.end(), [](const point &a, const point &b) { return a.x - a.y < b.x - b.y; });
    S1.reset();
    S2.reset();
    
    // x - yの昇順
    for (int i = 0; i < N; i++) {
        const point &p = P[i];
        // 0, x - yがpより小さく, xがp以上
        auto [dist, to] = S1.prod(N - 1 - p.cx);
        if (to != -1) E.push_back({dist - p.x - p.y, p.id, to});

        // 5, x - yがpより小さく, yがp以下
        std::tie(dist, to) = S2.prod(p.cy);
        if (to != -1) E.push_back({dist + p.x + p.y, p.id, to});

        S1.apply(N - 1 - p.cx, {p.x + p.y, p.id});
        S2.apply(p.cy, {-p.x - p.y, p.id});
    }
    S1.reset();
    S2.reset();
    
    // x - yの降順
    for (int i = N - 1; i >= 0; i--) {
        const point &p = P[i];
        // 4, x - yがpより大きく, xがp以下
        auto [dist, to] = S1.prod(p.cx);
        if (to != -1) E.push_back({dist + p.x + p.y, p.id, to});

        // 1, x + yがpより大きく, yがp以上
        std::tie(dist, to) = S2.prod(N - 1 - p.cy);
        if (to != -1) E.push_back({dist - p.x - p.y, p.id, to});

        S1.apply(p.cx, {-p.x - p.y, p.id});
        S2.apply(N - 1 - p.cy, {p.x + p.y, p.id});
    }
    return E;
};
#endif