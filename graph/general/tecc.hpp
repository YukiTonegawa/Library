#ifndef _TECC_H_
#define _TECC_H_
#include <vector>
#include <cassert>
#include <algorithm>
#include "../base/csr.hpp"

struct tecc {
    int _n;
    struct edge { int to; };
    std::vector<std::pair<int, edge>> edges;
    
    explicit tecc(int n) : _n(n) {}

    int num_vertices() { return _n; }

    void add_edge(int from, int to) { 
        edges.push_back({from, {to}}); 
        edges.push_back({to, {from}});
    }

    // @return pair of (# of tecc, tecc id)
    std::pair<int, std::vector<int>> tecc_ids() {
        auto g = csr<edge>(_n, edges);
        int now_ord = 0, group_num = 0;
        std::vector<int> visited, low(_n), ord(_n, -1), ids(_n);
        visited.reserve(_n);
        auto dfs = [&](auto self, int v, int p) -> void {
            low[v] = ord[v] = now_ord++;
            visited.push_back(v);
            bool rev_guard = true;
            for (int i = g.start[v]; i < g.start[v + 1]; i++) {
                auto to = g.elist[i].to;
                if (to == p && rev_guard) {
                    rev_guard = false;
                    continue;
                }
                if (ord[to] == -1) {
                    self(self, to, v);
                    low[v] = std::min(low[v], low[to]);
                } else {
                    low[v] = std::min(low[v], ord[to]);
                }
            }
            if (low[v] == ord[v]) {
                while (true) {
                    int u = visited.back();
                    visited.pop_back();
                    ord[u] = _n;
                    ids[u] = group_num;
                    if (u == v) break;
                }
                group_num++;
            }
        };
        for (int i = 0; i < _n; i++) {
            if (ord[i] == -1) dfs(dfs, i, -1);
        }
        for (auto& x : ids) {
            x = group_num - 1 - x;
        }
        return {group_num, ids};
    }

    // グループごと
    std::vector<std::vector<int>> tecc_group() {
        auto ids = tecc_ids();
        int group_num = ids.first;
        std::vector<int> counts(group_num);
        for (auto x : ids.second) counts[x]++;
        std::vector<std::vector<int>> groups(ids.first);
        for (int i = 0; i < group_num; i++) {
            groups[i].reserve(counts[i]);
        }
        for (int i = 0; i < _n; i++) {
            groups[ids.second[i]].push_back(i);
        }
        return groups;
    }
};
#endif