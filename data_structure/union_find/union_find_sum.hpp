#ifndef _UNION_FIND_SUM_H_
#define _UNION_FIND_SUM_H_
#include <numeric>
#include <vector>
#include <stack>
#include <queue>

// rollbackなし経路圧縮
template<typename S>
struct union_find_sum {
  private:
    int N;
    std::vector<int> sz;
    std::vector<S> val, sum;
    std::vector<S> lazy;

  public:
    union_find_sum() {}
    union_find_sum(int n) : N(n), sz(n, -1), val(n, 0), sum(val), lazy(n, 0) {}
    union_find_sum(const std::vector<S> &v): N(v.size()), sz(N, -1), val(v), sum(v), lazy(N, 0) {}
    
    int find(int u) {
        if (sz[u] < 0) return u;
        int p = find(sz[u]);
        if (p != sz[u]) lazy[u] += lazy[sz[u]];
        return sz[u] = p;
    }

    bool same(int u, int v) {
        return find(u) == find(v);
    }

    void unite(int u, int v) {
        u = find(u), v = find(v);
        if (u == v) return;
        if (sz[v] < sz[u]) std::swap(u, v);
        lazy[v] -= lazy[u];
        sum[u] += sum[v];
        sz[u] += sz[v];
        sz[v] = u;
    }

    // aを含む連結成分のサイズ
    int size(int a) {
        return -sz[find(a)];
    }

    // V[u] += x
    void apply(int u, S x) {
        val[u] += x;
        while (u >= 0) {
            sum[u] += x;
            u = sz[u];
        }
    }

    // V[u]
    S get(int u){
        S res = val[u];
        while (u >= 0) {
            res += lazy[u];
            u = sz[u];
        }
        return res;
    }

    // uを含む連結成分にxを作用させる
    void apply_cc(int u, S x) {
        u = find(u);
        lazy[u] += u;
    }

    // uを含む連結成分の和
    S prod_cc(int u) {
        u = find(u);
        return sum[u] + lazy[u] * (-sz[u]);
    }
};
#endif