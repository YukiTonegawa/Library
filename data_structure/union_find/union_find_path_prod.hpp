#ifndef _UNION_FIND_PATH_PROD_H_
#define _UNION_FIND_PATH_PROD_H_
#include <vector>
#include <iostream>
#include <numeric>

template<typename T, T (*op)(T, T)>
struct union_find_path_prod {
    std::vector<int> sz;
    std::vector<T> val;
    int cc;
    
    union_find_path_prod(int n, T x) : union_find_path_prod(std::vector<T>(n, x)) {}
    union_find_path_prod(const std::vector<T> _val) : sz(_val.size(), -1), val(_val), cc(_val.size()) {}
    
    // uを含む連結成分の根
    int find(int u) {
        if (sz[u] < 0) return u;
        int p = find(sz[u]);
        if (p != sz[u]) val[u] = op(val[sz[u]], val[u]);
        return sz[u] = p;
    }

    // op(root(v), ... par(v), v)
    T path_prod(int v) {
        int p = find(v);
        return p == v ? val[v] : op(val[p], val[v]);
    }

    // uを含む連結成分のサイズ
    int size(int u) {
        return -sz[find(u)];
    }
    
    // u, vが同じ連結成分か
    bool same(int u, int v) {
        return find(u) == find(v);
    }

    // uをvの親にする(vにすでに親があると壊れる)
    void unite(int u, int v) {
        assert(sz[v] < 0);
        int r = find(u);
        if (r == v) return;
        cc--;
        sz[r] += sz[v];
        sz[v] = u;
    }

    // 連結成分の数
    int count_cc() {
        return cc;
    }

    // 連結成分ごとに列挙
    std::vector<std::vector<int>> enumerate() {
        int n = sz.size();
        std::vector<std::vector<int>> res(n);
        for (int i = 0; i < n; i++) res[find(i)].push_back(i);
        return res;
    }
};
#endif