#ifndef _BIPARTITE_COMPLEMENT_
#define _BIPARTITE_COMPLEMENT_
#include <vector>
#include <queue>
#include "../../data_structure/hash_table/hash_set.hpp"
#include "../../data_structure/ordered_set/nazoki.hpp"
#include "../../math/data_structure/relation_union_find_mod2.hpp"

struct bipartite_complement_graph {
  private:
    int N;
    hash_set<long long> G;
    relation_union_find_mod2 uf;
    std::vector<std::pair<std::vector<int>, std::vector<int>>> cc;
    long long encode(int a, int b) {
        if (a > b) std::swap(a, b);
        return ((long long)a << 32) + b;
    }

  public:
    bipartite_complement_graph(int _N) : N(_N), G(2 * N), uf(N) {}

    void add_edge(int a, int b) {
        G.insert(encode(a, b));
    }

    // 補グラフが二部グラフか判定
    bool solve() {
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
                        uf.add_relation(v, u, 1);
                        unused.erase(u);
                        q.push(u);
                    }
                    u = unused.gt(u);
                }
            }
        };

        for (int i = 0; i < N; i++) {
            if (unused.find(i)) {
                bfs(i);
            }
        }

        cc.resize(N);

        for (int i = 0; i < N; i++) {
            int r = uf.find(i);
            if (!uf.relation(r, i).second) {
                cc[r].first.push_back(i);
            } else {
                cc[r].second.push_back(i);
            }
        }
        for (int r = 0; r < N; r++) {
            {
                int S = cc[r].first.size();
                for (int i = 0; i < S; i++) {
                    int x = cc[r].first[i];
                    for (int j = i + 1; j < S; j++) {
                        int y = cc[r].first[j];
                        if (!G.find(encode(x, y))) {
                            return false;
                        }
                    }
                }
            }
            {
                int S = cc[r].second.size();
                for (int i = 0; i < S; i++) {
                    int x = cc[r].second[i];
                    for (int j = i + 1; j < S; j++) {
                        int y = cc[r].second[j];
                        if (!G.find(encode(x, y))) {
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    }

    // a_u = a_v ^ aという式があるか, ある場合そのa
    // 先にsolveを呼ぶ
    std::pair<bool, bool> same(int u, int v) {
        return uf.relation(u, v);
    }

    // 条件を満たす二部グラフのうち1つを返す
    // 先にsolveを呼ぶ
    std::pair<std::vector<int>, std::vector<int>> get_graph() {
        std::vector<int> L, R;
        for (int i = 0; i < N; i++) {
            L.insert(L.end(), cc[i].first.begin(), cc[i].first.end());
            R.insert(R.end(), cc[i].second.begin(), cc[i].second.end());
        }
        return {L, R};
    }
};
#endif