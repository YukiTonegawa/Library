#ifndef _CHORDAL_H_
#define _CHORDAL_H_
#include <vector>
#include <queue>
#include <cassert>
#include <unordered_set>
#include "../base/csr.hpp"
#include "../base/edge.hpp"

template<typename edge = int>
struct chordal_graph_recognition {
    int N;
    std::vector<int> peo;
    void mcs(const csr<edge> &G) {
        std::vector<int> degY(N, 0);
        std::vector<std::unordered_set<int>> V(N + 1);
        V[0] = std::unordered_set<int>(2 * N);
        for (int i = 0; i < N; i++) V[0].insert(i);
        int d = 0;
        while (true) {
            if (!V[d + 1].empty()) d++;
            while (d >= 0 && V[d].empty()) d--;
            if (d < 0) break;
            auto itr = V[d].begin();
            int v = *itr;
            V[d].erase(itr);
            degY[v] = -1;
            peo.push_back(v);
            for (int i = G.begin(v); i < G.end(v); i++) {
                int u = G.elist[i];
                int &du = degY[u];
                if (du == -1) continue;
                V[du].erase(u);
                V[++du].insert(u);
            }
        }
        std::reverse(peo.begin(), peo.end());
    }

    chordal_graph_recognition() {}

    std::pair<bool, std::vector<int>> is_chordal(const csr<edge> &G) {
        N = G.N();
        mcs(G);
        int a = -1, b = -1, c = -1;

        std::vector<std::unordered_set<int>> S;
        for (int i = 0; i < N; i++) {
            S.push_back(std::unordered_set<int>(2 * (G.end(i) - G.begin(i))));
            for (int j = G.begin(i); j < G.end(i); j++) {
                int k = G.elist[j];
                S[i].insert(k);
            }
        }

        std::vector<int> used(N, false);

        auto find = [&]() -> void {
            for (int v : peo) {
                std::vector<int> U;
                used[v] = true;
                for (int j = G.begin(v); j < G.end(v); j++) {
                    int k = G.elist[j];
                    if (used[k]) continue;
                    for (int u : U) {
                        if (S[u].find(k) == S[u].end()) {
                            std::tie(a, b, c) = {v, u, k};
                            return;
                        }
                    }
                    U.push_back(k);
                }
            }
        };
        find();
        if (a == -1) return {true, {}};
        std::fill(used.begin(), used.end(), false);
        used[a] = 1;
        std::vector<int> par(N, -1);
        par[c] = a, par[a] = b;
        std::queue<int> que;
        for (int i = G.begin(a); i < G.end(a); i++) {
            int to = G.elist[i];
            used[to] = 1;
        }
        que.push(c);
        while (!que.empty() && par[b] == -1) {
            int v = que.front();
            que.pop();
            for (int i = G.begin(v); i < G.end(v); i++) {
                int t = G.elist[i];
                if (t == b) {
                    par[b] = v;
                    break;
                }
                if (used[t]) continue;
                used[t] = true;
                par[t] = v;
                que.push(t);
            }
        }
        std::vector<int> cycle;
        int v = b;
        while (true) {
            cycle.push_back(v);
            v = par[v];
            if (v == b) break;
        } 
        return {false, cycle};
    }
};
#endif