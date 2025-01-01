#ifndef _PERSISTENT_DEQUE_STRING_H_
#define _PERSISTENT_DEQUE_STRING_H_
#include <deque>
#include <cassert>
#include "tree_path_string.hpp"

// バージョン0は空の列とする
template<typename T, size_t MAX_LEN = 2000000>
struct ds_deque_persistent {
    using rh = rolling_hash_base<MAX_LEN>;
    using ull = uint64_t;
    tree_path_string<MAX_LEN> tree;
    std::vector<std::vector<std::tuple<int, int, T>>> G;
    std::vector<std::pair<int, int>> border;

    ds_deque_persistent() : G(1) {}

    int push_front(int ver, T x) {
        int id = G.size();
        G[ver].push_back({id, 0, x});
        G.push_back({});
        return id;
    }

    int pop_front(int ver) {
        int id = G.size();
        G[ver].push_back({id, 1, std::numeric_limits<T>::max()});
        G.push_back({});
        return id;
    }

    int push_back(int ver, T x) {
        int id = G.size();
        G[ver].push_back({id, 2, x});
        G.push_back({});
        return id;
    }

    int pop_back(int ver) {
        int id = G.size();
        G[ver].push_back({id, 3, std::numeric_limits<T>::max()});
        G.push_back({});
        return id;
    }

    void build() {
        int M = G.size();
        std::vector<int> par(M, -1);
        std::deque<int> path;
        std::vector<T> val(M, std::numeric_limits<T>::max());
        border.resize(M);

        auto dfs = [&](auto &&dfs, int v) -> void {
            if (path.empty()) {
                border[v] = {0, 0};
            } else {
                border[v] = {path.front(), path.back()};
            }

            for (auto [id, op, x] : G[v]) {
                val[id] = x;
                if (op == 0) {
                    if (!path.empty()) par[id] = path.front();
                    path.push_front(id);
                    dfs(dfs, id);
                    path.pop_front();
                } else if (op == 1) {
                    int top = (!path.empty() ? path.front() : -1);
                    if (top != -1) path.pop_front();
                    dfs(dfs, id);
                    if (top != -1) path.push_front(top);
                } else if (op == 2) {
                    if (!path.empty()) par[id] = path.back();
                    path.push_back(id);
                    dfs(dfs, id);
                    path.pop_back();
                } else {
                    int top = (!path.empty() ? path.back() : -1);
                    if (top != -1) path.pop_back();
                    dfs(dfs, id);
                    if (top != -1) path.push_back(top);
                }
            }
        };
        dfs(dfs, 0);
        std::vector<std::pair<int, int>> E;
        for (int i = 1; i < M; i++) {
            if (par[i] == -1) {
                E.push_back({0, i});
                E.push_back({i, 0});
            } else {
                E.push_back({par[i], i});
                E.push_back({i, par[i]});
            }
        }
        tree = tree_path_lcp<MAX_LEN>(csr<int>(M, E), val);
    }

    int size(int ver) const {
        auto [a, b] = border[ver];
        return tree.dist(a, b);
    }

    // hash(Q_{ver}[k])
    ull get(int ver, int k) const {
        auto [a, b] = border[ver];
        return tree.get(tree.jump_on_tree(a, b, k));
    }

    // hash(Q_{ver}[l, r))
    ull hash_range(int ver, int l, int r) const {
        auto [a, b] = border[ver];
        if (l == r) return 0;
        int c = tree.jump_on_tree(a, b, l);
        int d = tree.jump_on_tree(a, b, r - 1);
        assert(l < r && c != -1 && d != -1);
        return tree.hash_path(c, d);
    }

    // hash(Q_{ver})
    ull hash_all(int ver) const {
        auto [a, b] = border[ver];
        return tree.hash_path(a, b);
    }

    int lcp(int ver1, int ver2) const {
        auto [a, b] = border[ver1];
        auto [c, d] = border[ver2];
        return tree.lcp(a, b, c, d);
    }

    int compare(int ver1, int ver2) const {
        auto [a, b] = border[ver1];
        auto [c, d] = border[ver2];
        return tree.compare(a, b, c, d);
    }

    // 辞書順の昇順にバージョンをソート
    std::vector<int> sort_ver(std::vector<int> V) {
        std::sort(V.begin(), V.end(), [&](int a, int b) { return compare(a, b) == -1; });
        return V;
    }
};
#endif