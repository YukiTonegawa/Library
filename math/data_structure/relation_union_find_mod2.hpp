#ifndef _RELATION_UNION_FIND_H_
#define _RELATION_UNION_FIND_H_
#include <vector>
#include <algorithm>
#include <numeric>

struct relation_union_find_mod2 {
  private:
    int num_cc;
    std::vector<int> sz, diff;
    std::vector<bool> decided, ans;
  
  public:
    relation_union_find_mod2(int n): num_cc(n), sz(n, -1), diff(n, 0), decided(n, 0), ans(n, 0) {}

    int find(int u) {
        if (sz[u] < 0) return u;
        int p = find(sz[u]);
        if (sz[u] != p) diff[u] ^= diff[sz[u]];
        return sz[u] = p;
    }
    
    int size(int u) {
        return -sz[find(u)];
    }
    
    bool same(int u, int v) {
        return find(u) == find(v);
    }
    
    // 条件 x_u = x_v ^ a を追加可能か
    bool check_relation(int u, int v, bool a) {
        int ur = find(u), vr = find(v);
        if (ur == vr) return (diff[v] ^ a) == diff[u];
        int c = diff[v] ^ diff[u] ^ a;
        if (decided[ur] && decided[vr] && (ans[vr] ^ c) != ans[ur]) return false;
        return true;
    }

    // 条件 x_u = x_v ^ a を追加
    bool add_relation(int u, int v, bool a) {
        int ur = find(u), vr = find(v);
        if (ur == vr) return (diff[v] ^ a) == diff[u];
        if (sz[ur] < sz[vr]) {
            std::swap(u, v);
            std::swap(ur, vr);
        }
        // ur = vr ^ c
        int c = diff[v] ^ diff[u] ^ a;
        if (decided[ur] && decided[vr] && (ans[vr] ^ c) != ans[ur]) return false;
        num_cc--;
        sz[vr] += sz[ur];
        sz[ur] = vr;
        diff[ur] = c;
        if (decided[ur]) {
            decided[vr] = true;
            ans[vr] = ans[ur] ^ c;
        }
        return true;
    }

    // 条件 x_u = a を追加可能か
    bool check_const(int u, bool a) {
        int ur = find(u);
        if (!decided[ur]) return true;
        return (ans[ur] ^ diff[u]) == a;
    }

    // 条件 x_u = a を追加
    bool add_const(int u, bool a) {
        int ur = find(u);
        if (!decided[ur]) {
            ans[ur] = a ^ diff[u];
            decided[ur] = true;
        }
        return (ans[ur] ^ diff[u]) == a;
    }

    // {確定しているか, そうでない場合, その値}
    std::pair<bool, bool> get(int u) {
        int ur = find(u);
        if (!decided[ur]) return {false, 0};
        return {true, (ans[ur] ^ diff[u])};
    }

    // u = v ^ aという式が存在するか, 存在する場合そのa
    std::pair<bool, bool> relation(int u, int v) {
        int ur = find(u), vr = find(v);
        if (ur != vr) return {false, 0};
        return {true, diff[u] ^ diff[v]};
    }

    // 連結成分の数
    int count_cc() {
        return num_cc;
    }
};
#endif