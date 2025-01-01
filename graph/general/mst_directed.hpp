#ifndef _MST_DIRECTED_H_
#define _MST_DIRECTED_H_
#include <vector>
#include <cassert>
#include "../../data_structure/heap/all_add_leftist_heap_map.hpp"
#include "../../data_structure/union_find/union_find_sum_rollback.hpp"

template<typename T, T large>
struct mst_directed {
  public:
    struct edge {
        int s, t;
        T w;
    };
  private:
    using Heap = all_add_leftist_heap_map<T, int, std::less<T>>;
    int N, r;
    std::vector<Heap> H;
    std::vector<edge> E;

  public:
    mst_directed(int _N, int _r) : N(_N), r(_r), H(N) {}

    int add_edge(int s, int t, T w) {
        int id = E.size();
        E.push_back({s, t, w});
        if (s != t && (w == large || t != r)) H[t].push(w, id);
        return id;
    }

    edge get_edge(int id) const {
        return E[id];
    }

    // 辺番号を返す
    // rから任意のノードに辿り着ける場合mst
    // そうでない場合, 連結成分が最小になる中で重みが最小になる森
    // large >= 全ての辺の重みの和として、(連結成分数-1)*large + 重みの和
    std::vector<int> solve() {
        std::vector<bool> used(N, false);
        std::vector<int> path, edges, res;
        union_find_sum_rollback<int> uf(N);
        for (int i = 0; i < N; i++) {
            add_edge(i, r, large);
            add_edge(r, i, large);
        }
        auto dfs = [&](auto &&dfs, int v) -> void {
            if (uf.size(0) == N) {
                uf.apply(r, 1);
                return;
            }
            if (used[v]) {
                std::vector<int> tmp;
                int r = path.size(), l = r - 1;
                while (path[l] != v) uf.unite(v, path[l--]);
                v = uf.find(v);
                for (int i = 0; i < r - l; i++) {
                    int u = path.back();
                    if (u != v) H[v].meld(H[u]);
                    path.pop_back();
                    tmp.push_back(edges.back());
                    edges.pop_back();
                }
                used[v] = false;
                dfs(dfs, v);
                for (int i = l + 1; i < r; i++) uf.rollback();
                for (int id : tmp) {
                    auto e = E[id];
                    if (uf.prod_cc(e.t) == 0) {
                        res.push_back(id);
                        uf.apply(e.t, 1);
                    }
                }
            } else {
                used[v] = true;
                path.push_back(v);
                while (!H[v].empty()) {
                    auto [w, id] = H[v].top();
                    if (uf.same(v, get_edge(id).s)) H[v].pop();
                    else break;
                }
                auto [w, id] = H[v].top();
                edges.push_back(id);
                H[v].pop();
                H[v].all_add(-w);
                dfs(dfs, uf.find(get_edge(id).s));
            }
        };
        dfs(dfs, r);
        return res;
    }
};
#endif