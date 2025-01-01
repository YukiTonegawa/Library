#ifndef _CYCLE_DETECTION_H_
#define _CYCLE_DETECTION_H_
#include <queue>
#include "../base/edge.hpp"
#include "../base/csr.hpp"

// O(V + E)
// ショートカットが無い閉路を1つ返す, 閉路が無い場合は空のvector
// accept_self_loop := 自己ループを閉路とするか
// 多重辺があっても良い
// 無向辺の場合辺にidをつけて逆流しないようにする 
template<typename edge>
std::vector<edge> cycle_detection(const csr<edge> &g, bool accept_self_loop) {
    int N = g.N();
    if (accept_self_loop) {
        for (int i = 0; i < N; i++) {
            for (int j = g.begin(i); j < g.end(i); j++) {
                int to = g.elist[j];
                if (to == i) return {g.elist[j]};
            }
        }
    }
    std::vector<bool> used(N, false);
    std::vector<int> in(N, -1);
    std::vector<edge> res;
    std::deque<edge> E;

    // ショートカットを消す
    auto shrink = [&]() -> void {
        std::fill(in.begin(), in.end(), -1);
        int s = E.front().from(), v = s, ord = 0;
        int last_id = -1;
        while (true) {
            in[v] = ord++;
            for (int i = g.begin(v); i < g.end(v); i++) {
                int to = g.elist[i];
                int id = g.elist[i].id();
                if (to != v && in[to] != -1 && (id == -1 || id != last_id)) {
                    res.erase(res.begin(), res.begin() + in[to]);
                    res.push_back(g.elist[i]);
                    return;
                }
            }
            res.push_back(E.front());
            v = E.front().to();
            last_id = E.front().id();
            E.pop_front();
        }
    };

    auto dfs = [&](auto &&dfs, int v, int last_id) -> void {
        if (in[v] >= 0) {
            while (E.front().from() != v) E.pop_front();
            shrink();
            return;
        }
        in[v] = E.size();
        for (int i = g.begin(v); i < g.end(v); i++) {
            int to = g.elist[i];
            if (to == v) continue;
            int id = g.elist[i].id();
            if (res.empty() && !used[to] && (id == -1 || id != last_id)) {
                E.push_back(g.elist[i]);
                dfs(dfs, to, id);
                if (!E.empty()) E.pop_back();
            }
        }
        used[v] = true;
        in[v] = -1;
    };

    for (int s = 0; s < N; s++) {
        if (used[s]) continue;
        dfs(dfs, s, -1);
        if (!res.empty()) return res;
    }
    return {};
}
#endif