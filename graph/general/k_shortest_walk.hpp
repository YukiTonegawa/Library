#ifndef _K_SHORTEST_WALK_H_
#define _K_SHORTEST_WALK_H_
#include <numeric>
#include <vector>
#include <cassert>
#include <queue>
#include "../../data_structure/persistent/persistent_leftist_heap.hpp"

template<typename T>
struct k_shortest_walk {
    int N;
    std::vector<std::tuple<int, int, T>> E;

    k_shortest_walk(int N) : N(N) {}

    void add_edge(int s, int t, T w) {
        E.push_back({s, t, w});
    }

    // [0, k)番目に小さいs-t walkの長さ
    // k個存在しない場合はあるだけ返す(特にs-t walkがない場合は空配列を返す)
    std::vector<T> solve(int s, int t, int k) {
        using TI = std::pair<T, int>;
        using PQ = persistent_leftist_heap<TI, std::less<TI>>;
        static constexpr T inf = std::numeric_limits<T>::max() / 2;
        std::vector<T> P(N, inf);
        std::vector<bool> eused(int(E.size()), 0);
        std::vector<int> ord, par(N, -1), paredge(N, -1);
        std::vector<PQ> H(N);

        {
            std::vector<std::vector<std::tuple<int, T, int>>> Grev(N);
            for (int i = 0; i < int(E.size()); i++) {
                auto [a, b, w] = E[i];
                Grev[b].push_back({a, w, i});
            }
            std::priority_queue<TI, std::vector<TI>, std::greater<TI>> q;
            q.push({0, t});
            P[t] = 0;
            while (!q.empty()) {
                auto [d, v] = q.top();
                q.pop();
                if (P[v] != d) continue;
                ord.push_back(v);
                for (auto [a, w, id] : Grev[v]) {
                    T d = P[v] + w;
                    if (P[a] > d) {
                        P[a] = d;
                        par[a] = v;
                        paredge[a] = id;
                        q.push({d, a});
                    }
                }
            }
        }
        if (P[s] == inf) return {};

        {

            std::vector<std::vector<std::pair<int, T>>> G(N);
            for (int i = 0; i < N; i++) {
                if (par[i] == -1) continue;
                eused[paredge[i]] = 1;
            }
            for (int i = 0; i < int(E.size()); i++) {
                if (eused[i]) continue;
                auto [a, b, w] = E[i];
                G[a].push_back({b, w});
            }

            for (int v : ord) {
                if (P[v] == inf) continue;
                if (par[v] != -1) H[v] = H[par[v]];
                for (auto [b, w] : G[v]) {
                    if (P[b] == inf) continue;
                    H[v] = H[v].push({w - P[v] + P[b], b});
                }
            }
        }

        std::vector<T> res;
        {
            using TVH = std::pair<T, PQ>;
            struct cmp {
                bool operator ()(const TVH &a, const TVH &b) const {
                    return std::get<0>(a) > std::get<0>(b);
                }
            };
            std::priority_queue<TVH, std::vector<TVH>, cmp> q;
            PQ h0 = PQ().push({P[s], s});
            q.push({P[s], h0});
            while (!q.empty() && res.size() < k) {
                auto [w, h] = q.top();
                q.pop();
                res.push_back(w);
                if (h.empty()) continue;
                T prew = w - h.top().first;
                PQ l = h._left();
                if (!l.empty()) {
                    auto [d, u] = l.top();
                    q.push({prew + d, l});
                }
                PQ r = h._right();
                if (!r.empty()) {
                    auto [d, u] = r.top();
                    q.push({prew + d, r});
                }
                auto [d, u] = h.top();
                if (!H[u].empty()) {
                    q.push({w + H[u].top().first, H[u]});
                }
            }
        }

        return res;
    }
};
#endif