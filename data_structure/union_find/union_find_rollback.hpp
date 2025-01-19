#ifndef _UNION_FIND_ROLLBACK_H_
#define _UNION_FIND_ROLLBACK_H_
#include <vector>
#include <queue>
#include <cassert>

template<bool use_enumerate = false>
struct union_find_rollback {
  private:
    int N, cc;
    std::vector<int> sz, dup, history;
    std::vector<std::vector<int>> ch;

public:
    union_find_rollback() {}
    union_find_rollback(int n) : N(n), cc(N), sz(N, -1), dup(N, 0) {
        if constexpr (use_enumerate) ch.resize(N);
    }
  
    int find(int u) {
        while (sz[u] >= 0) u = sz[u];
        return u;
    }

    bool same(int u, int v) {
        return find(u) == find(v);
    }

    void unite(int u, int v) {
        u = find(u), v = find(v);
        if (u == v) {
            dup[u]++;
            history.push_back(u);
            return;
        }
        if (sz[v] < sz[u]) std::swap(u, v);
        history.push_back(u);
        history.push_back(v);
        history.push_back(sz[v]);
        cc--;
        if constexpr (use_enumerate) {
            ch[u].push_back(v);
        }
        sz[u] += sz[v];
        sz[v] = u;
        dup[u] += dup[v];
    }

    // aを含む連結成分のサイズ
    int size(int a) {
        return -sz[find(a)];
    }

    // 直前の辺を取り除いたことで連結成分が増えたか
    bool rollback() {
        assert(!history.empty());
        int s = history.back();
        history.pop_back();
        if (s >= 0) {
            dup[s]--;
            return false;
        }
        int c = history.back();
        history.pop_back();
        int p = history.back();
        history.pop_back();
        cc++;
        dup[p] -= dup[c];
        sz[c] = s;
        sz[p] -= s;
        if constexpr (use_enumerate){
            ch[p].pop_back();
        }
        return true;
    }

    // 頂点uを含む連結成分の全要素
    std::vector<int> enumerate(int u) {
        assert(use_enumerate);
        std::vector<int> res;
        std::queue<int> q;
        q.push(find(u));
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            res.push_back(v);
            for (int c : ch[v]) q.push(c);
        }
        return res;
    }
    
    // uを含む連結成分が木か
    int is_tree(int u) {
        return dup[find(u)] == 0;
    }

    // uを含む連結成分からあと何本辺を取れば木になるか
    int duplicated(int u) {
        return dup[find(u)];
    }

    // 連結成分の数
    int count_cc() {
        return cc;
    }
};
#endif
