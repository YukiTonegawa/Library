#ifndef _COMPLEMENT_GRAPH_CC_H_
#define _COMPLEMENT_GRAPH_CC_H_
#include <vector>
#include <queue>
#include "../../data_structure/hash_table/hash_set.hpp"
#include "../../data_structure/ordered_set/nazoki.hpp"
#include "../../data_structure/union_find/union_find.hpp"

struct connected_component_complement_graph {
  private:
    int N;
    hash_set<long long> G;
    long long encode(int a, int b) {
        if (a > b) std::swap(a, b);
        return ((long long)a << 32) + b;
    }

  public:
    connected_component_complement_graph(int _N) : N(_N), G(2 * N) {}

    void add_edge(int a, int b) {
        G.insert(encode(a, b));
    }

    std::vector<std::vector<int>> solve() {
        union_find uf(N);
        nazotree<4> unused;
        for (int i = 0; i < N; i++) unused.insert(i);

        auto bfs = [&](int root) -> void {
            std::queue<int> q;
            q.push(root);
            unused.erase(root);
            while (!q.empty()) {
                int v = q.front();
                q.pop();
                int u = unused.ge(0);
                while (u != -1) {
                    if (!G.find(encode(v, u))) {
                        uf.unite(root, u);
                        unused.erase(u);
                        q.push(u);
                    }
                    u = unused.gt(u);
                }
            }
        };

        for (int i = 0; i < N; i++) if (unused.find(i)) bfs(i);
        std::vector<std::vector<int>> res;
        for (auto cc : uf.enumerate()) {
            if (!cc.empty()) res.push_back(cc);
        }
        return res;
    }
};
#endif