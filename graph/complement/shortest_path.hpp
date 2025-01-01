#ifndef _COMPLEMENT_SHORTEST_PATH_H_
#define _COMPLEMENT_SHORTEST_PATH_H_
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <queue>

// 無向グラフの補グラフ上の最短距離
struct shortest_path_complement_graph {
  private:
    int N, _s;
    std::unordered_set<long long> E;
    std::vector<int> dist, par;

    bool find(int a, int b) {
        if (a > b) std::swap(a, b);
        return E.find(((long long)a << 32) + b) != E.end();
    }
    
  public:
    shortest_path_complement_graph(int _N) : N(_N), dist(N), par(N) {}

    void add_edge(int a, int b) {
        if (a > b) std::swap(a, b);
        E.insert(((long long)a << 32) + b);
    }

    // O(N + M)
    void build(int s) {
        _s = s;
        std::fill(dist.begin(), dist.end(), std::numeric_limits<int>::max());
        std::fill(par.begin(), par.end(), -1);
        std::unordered_set<int> unused;
        for (int i = 0; i < N; i++) if (i != s) unused.insert(i);
        std::queue<int> q;
        q.push(s);
        dist[s] = 0;
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            auto itr = unused.begin();
            while (itr != unused.end()) {
                if (find(v, *itr)) {
                    itr++;
                } else {
                    dist[*itr] = dist[v] + 1;
                    par[*itr] = v;
                    q.push(*itr);
                    itr = unused.erase(itr);
                }
            }
        }
    }

    std::vector<int> path(int t) const {
        assert(!dist.empty());
        if (par[t] == -1) return {};
        std::vector<int> res{t};
        while (t != _s) {
            t = par[t];
            res.push_back(t);
        }
        std::reverse(res.begin(), res.end());
        return res;
    }

    int operator [](int i) const { return dist[i]; }
};
#endif