#ifndef _REROOTING_H_
#define _REROOTING_H_
#include <vector>
#include <cassert>
#include "../base/csr.hpp"
#include "../../data_structure/hash_table/hash_map.hpp"

// 辺{s, t}がある時辺{t, s}が必要
template<typename edge, typename S, S (*up)(edge, S), S (*sibling)(S, S), S (*parent)(S, S), S (*e)()>
struct rerooting {
  private:
    int _root;
    std::vector<std::vector<S>> suml, sumr;
    std::vector<S> X;
    hash_map<uint64_t, int> E;
    static constexpr uint64_t encode(int a, int b) {
        return (uint64_t(a) << 32) + b;
    }
    void build(csr<edge> G) {
        auto dfs_up = [&](auto &&dfs_up, int v, int p) -> S {
            suml[v].resize(G.end(v) - G.begin(v), e());
            S res = e();
            int par_idx = -1;
            for (int i = G.begin(v), j = 0; i < G.end(v); i++, j++) {
                int to = G[i];
                if (to == p) {
                    par_idx = j;
                    suml[v][j] = e();
                } else {
                    suml[v][j] = up(G[i], dfs_up(dfs_up, to, v));
                }
                res = sibling(res, suml[v][j]);
            }
            if (par_idx != -1) {
                std::swap(suml[v][par_idx], suml[v].back());
                std::swap(G[G.begin(v) + par_idx], G[G.end(v) - 1]);
            }
            return parent(X[v], res);
        };

        auto dfs_down = [&](auto &&dfs_down, int v, int p) -> void {
            int sz = G.end(v) - G.begin(v);
            sumr[v].resize(sz, e());
            for (int i = sz - 1; i >= 0; i--) {
                sumr[v][i] = suml[v][i];
                if (i + 1 < sz) {
                    sumr[v][i] = sibling(sumr[v][i], sumr[v][i + 1]);
                }
            }
            for (int i = 1; i < sz; i++) {
                suml[v][i] = sibling(suml[v][i], suml[v][i - 1]);
            }

            for (int i = 0; i < sz; i++) {
                int j = G.begin(v) + i;
                int to = G[j];
                if (to == p) continue;
                S left = (i == 0 ? e() : suml[v][i - 1]);
                S right = (i == sz - 1 ? e() : sumr[v][i + 1]);
                suml[to].back() = up(G[j], parent(X[v], sibling(left, right)));
                dfs_down(dfs_down, to, v);
            }
        };

        int N = G.N();
        suml.resize(N);
        sumr.resize(N);
        dfs_up(dfs_up, _root, -1);

        E = hash_map<uint64_t, int>(2 * N);
        for (int v = 0; v < N; v++) {
            for (int i = G.begin(v); i < G.end(v); i++) {
                int to = G[i];
                E.emplace(encode(v, to), i - G.begin(v));
            }
        }
        
        dfs_down(dfs_down, _root, -1);
    }

  public:
    rerooting() {}
    rerooting(const csr<edge> &_G, const std::vector<S> &_X, int _root = 0) : _root(_root), X(_X) { build(_G); }

    // vを根としたDP
    S prod(int v) const {
        return (suml[v].empty() ? X[v] : parent(X[v], suml[v].back()));
    }

    // _rootを根とするvの部分木のDP
    S prod_subtree(int v) const {
        if (v == _root) return prod(v);
        int len = suml[v].size();
        return (len == 1 ? X[v] : parent(X[v], suml[v][len - 2]));
    }

    // vの_root方向の部分木のDP
    S prod_parent(int v) const {
        if (v == _root) return X[v];
        return parent(X[v], sumr[v].back());
    }

    // pを根としたvの部分木のDP(辺p-vがないと壊れる)
    S prod_rerooting(int p, int v) {
        auto [f, i] = E.find(encode(v, p));
        assert(f);
        S left = (i == 0 ? e() : suml[v][i - 1]);
        S right = (i + 1 == sumr[v].size() ? e() : sumr[v][i + 1]);
        return parent(X[v], sibling(left, right));
    }
};

template<typename edge, typename T>
struct edpc_subtree {
    using S = T;
    using Edge = edge;
    static S e(){ return 1; }
    static S up(Edge e, S a) { return a + 1; }
    static S sibling(S a, S b) { return a * b; }
    static S parent(S p, S c) { return sibling(p, c); }
};

template<typename edge, typename T>
struct subtree_sum {
    using S = T;
    using Edge = edge;
    static S e(){ return 0; }
    static S up(Edge e, S a) { return a; }
    static S sibling(S a, S b) { return a + b; }
    static S parent(S p, S c) { return sibling(p, c); }
};

template<typename edge, typename T>
struct maxdist {
    using S = std::pair<T, int>;
    using Edge = edge;
    static S e() { return {0, 0}; } // 距離, 頂点番号
    static S up(Edge e, S a){ return { a.first + 1, a.second}; }
    static S sibling(S a, S b) { return std::max(a, b); }
    static S parent(S p, S c) { return sibling(p, c); }
};

template<typename edge, typename T>
struct mindist_to_marked_vertex {
    using S = T;
    using Edge = edge;
    static constexpr T inf = std::numeric_limits<T>::max() / 2;
    static S e() { return inf; } // マークされた頂点までの距離(マークされていない頂点はinf)
    static S up(Edge e, S a) { return std::min(a + 1, inf); }
    static S sibling(S a, S b) { return std::min(a, b); }
    static S parent(S p, S c) { return sibling(p, c); }
};

template<typename edge, typename T>
struct tree_path_composite_sum {
  using S = std::pair<T, T>;
  using Edge = edge;
  static constexpr S e() { return {0, 0}; }
  static constexpr S up(Edge e, S a) { return {a.first, e.weight().first * a.second + e.weight().second * a.first}; }
  static constexpr S sibling(S a, S b) { return {a.first + b.first, a.second + b.second}; }
  static constexpr S parent(S p, S c) { return sibling(p, c); }
};
#endif