#ifndef _PROJECT_SELECTION_H_
#define _PROJECT_SELECTION_H_
#include "dinic.hpp"

// O(N^2M)
// 最小カットがinfを超えない (最終的な答えが越えなくても最小カットが超える可能性がある)
template<typename T, T inf>
struct project_selection {
    int N, s, t;
    T add;
    dinic<T> G;
    project_selection() : G(0) {}
    project_selection(int _N) : N(_N), s(_N), t(_N + 1), add(0), G(_N + 2) {}

    // aがfaかつbが!faのとき+x(<0)
    void condition_diff(int a, int b, bool fa, T x) {
        assert(0 <= a && a < N);
        assert(0 <= b && b < N);
        assert(x <= 0);
        if (a == b || x == 0) return;
        if (!fa) {
            G.add_edge(a, b, -x);
        } else {
            G.add_edge(b, a, -x);
        }
    }

    // Vのうちどれか1つ以上がfなら+x(< 0)
    // 入れ子にできる : https://kanpurin.hatenablog.com/entry/moyasu-umeru#library
    void condition_either(const std::vector<int> &V, bool f, T x) {
        assert(x <= 0);
        if (x == 0) return;
        int w = G.add_vertex();
        if (!f) {
            G.add_edge(w, t, -x);
            for (int v : V) G.add_edge(v, w, inf);
        } else {
            G.add_edge(s, w, -x);
            for (int v : V) G.add_edge(w, v, inf);
        }
    }

    // aがfなら+x(任意)
    void condition_1_variable(int a, bool f, T x) {
        assert(0 <= a && a < N);
        if (x == 0) return;
        if (x > 0) {
            add += x;
            if (!f) G.add_edge(s, a, x);
            else G.add_edge(a, t, x);
        } else {
            if (!f) G.add_edge(a, t, -x);
            else G.add_edge(s, a, -x);
        }
    }

    // (fa, fb)が
    // (0, 0) -> +x0
    // (0, 1) -> +x1
    // (1, 0) -> +x2
    // (1, 1) -> +x3
    // 正/負でもいいが (x1 + x2 - x0 - x3) <= 0が必要
    void condition_2_variable(int a, int b, T x0, T x1, T x2, T x3) {
        assert(0 <= a && a < N);
        assert(0 <= b && b < N);
        assert(x1 + x2 - x0 + x3 <= 0);
        add += x0;
        condition_1_variable(a, true, x2 - x0);
        condition_1_variable(b, true, x3 - x2);
        condition_diff(a, b, false, x1 + x2 - x0 - x3);
    }

    // {infをcutしないような解があるか, 最大スコア, 各変数の値}
    std::tuple<bool, T, std::vector<bool>> max_score() {
        T f = G.max_flow(s, t, inf);
        if (f == inf) return {false, 0, {}};
        T score = add - f;
        return {true, score, G.min_cut(s)};
    }
};
#endif
