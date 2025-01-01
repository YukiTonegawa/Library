#ifndef _UNION_FIND_H_
#define _UNION_FIND_H_
#include <vector>
#include <iostream>
#include <numeric>

struct union_find {
    std::vector<int> sz;
    int cc;
    
    union_find(int n) : sz(n, -1), cc(n) {}
    
    // uを含む連結成分の根
    int find(int u) {
        if (sz[u] < 0) return u;
        return sz[u] = find(sz[u]);
    }

    // uを含む連結成分のサイズ
    int size(int u) {
        return -sz[find(u)];
    }
    
    // u, vが同じ連結成分か
    bool same(int u, int v) {
        return find(u) == find(v);
    }

    // 連結成分をマージ
    // u, vがすでに同じ連結成分 -> 0
    // u側が根になった -> 1
    // v側が根になった -> 2
    int unite(int u, int v) {
        u = find(u), v = find(v);
        if (u == v) return 0;
        int flag = 1;
        if (sz[u] > sz[v]) std::swap(u, v), flag++;
        cc--;
        sz[u] += sz[v];
        sz[v] = u;
        return flag;
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