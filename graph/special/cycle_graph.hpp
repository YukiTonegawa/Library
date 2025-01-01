#ifndef _CYCLE_GRAPH_H_
#define _CYCLE_GRAPH_H_
#include <vector>
#include <algorithm>
#include <numeric>
#include <cassert>
#include <unordered_set>
#include "../../data_structure/segtree/segtree.hpp"

// 頂点番号順に円環状に並んでいる
struct cycle_graph {
    static constexpr int inf = std::numeric_limits<int>::max();
    static constexpr int minf = std::numeric_limits<int>::min();
    using _S = std::pair<int, int>;
    static constexpr _S _op(_S a, _S b) { return _S{std::min(a.first, b.first), std::max(a.second, b.second)};}
    static constexpr _S _e() { return _S{inf, minf}; }
    int N;
    segtree<_S, _op, _e> ST;
    std::unordered_set<long long> E;
    cycle_graph(int N) : N(N) { ST = segtree<_S, _op, _e>(N); }

    // 辺abが頂点以外で交差する辺が存在するか(abも交差するとみなす)
    // 存在する場合その辺を返す, しない場合 {inf, minf}
    std::pair<int, int> check_intersect(int a, int b) {
        if (a > b) std::swap(a, b);
        assert(0 <= a && a < b && b < N);
        // 交差するパターン
        // パターン1 abが存在する
        // パターン2 [a + 1, b)から出る辺の(最小値 < a || 最大値 > b)
        // 1
        auto itr = E.find((((long long)a) << 32) + b);
        if (itr != E.end()) return {a, b};
        // 2
        auto [mn, mx] = ST.prod(a + 1, b);
        if (mn < a || mx > b) {
            int c = ST.max_right(a + 1, [&](_S x) { return a <= x.first && x.second <= b; });
            auto [cmin, cmax] = ST.get(c);
            if (cmin < a) return {cmin, c};
            if (cmax > b) return {c, cmax};
            assert(false);
        }
        return {inf, minf};
    }

    // 辺abを追加
    void add_edge(int a, int b) {
        if (a > b) std::swap(a, b);
        assert(0 <= a && a < b && b < N);
        auto [amin, amax] = ST.get(a);
        auto [bmin, bmax] = ST.get(b);
        ST.set(a, {std::min(b, amin), std::max(b, amax)});
        ST.set(b, {std::min(a, bmin), std::max(a, bmax)});
        E.insert((((long long)a) << 32) + b);
    }

    // a -> bの距離
    int dist(int a, int b) {
        int d = b - a;
        if (d < 0) d += N;
        return d;
    }

    // aのk個先
    int next(int a, int k = 1) {
        return (a + k) % N;
    }

    // aのk個前
    int prev(int a, int k = 1) {
        int res = (a - k) % N;
        if (res < 0) res += N;
        return res;
    }
};
#endif