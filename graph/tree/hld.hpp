#ifndef _HLD_H_
#define _HLD_H_
#include "../base/csr.hpp"

template<typename edge = int>
struct hld {
    int N, root;
    csr<edge> G;
    std::vector<int> sz, dep, par, head;
    std::vector<int> tour, in, out;

    hld() : N(0) {}
    hld(int root, const csr<edge> &_G) : N(_G.N()), root(root), G(_G), sz(N), dep(N), par(N), head(N), tour(N), in(N), out(N) {
        auto dfs_sz = [&](auto &&dfs_sz, int v, int p, int d) -> void {
            sz[v] = 1, dep[v] = d, par[v] = p;
            int L = G.begin(v);
            for (int i = L; i < G.end(v); i++) {
                int to = G.elist[i];
                if (to == p) continue;
                dfs_sz(dfs_sz, to, v, d + 1);
                sz[v] += sz[to];
                int hc = G.elist[L];
                if (hc == p || sz[hc] < sz[to]) {
                    std::swap(G.elist[L], G.elist[i]);
                }
            }
        };
        int t = 0;
        auto dfs_tour = [&](auto &&dfs_tour, int v, int p) -> void {
            in[v] = t;
            tour[t++] = v;
            int L = G.begin(v);
            for (int i = L; i < G.end(v); i++) {
                int to = G.elist[i];
                if (to == p) continue;
                if (i == L) {
                    head[to] = head[v];
                } else {
                    head[to] = to;
                }
                dfs_tour(dfs_tour, to, v);
            }
            out[v] = t;
        };
        head[root] = root;
        dfs_sz(dfs_sz, root, -1, 0);
        dfs_tour(dfs_tour, root, -1);
    }

    // k個上の祖先, k > depth[v]なら-1
    int la(int v, int k) const {
        if (dep[v] < k) return -1;
        while (true) {
            int u = head[v];
            if (in[v] - in[u] >= k) return tour[in[v] - k];
            k -= in[v] - in[u] + 1;
            v = par[u];
        }
    }

    int lca(int u, int v) const {
        while (true) {
            if (in[u] > in[v]) std::swap(u, v);
            if (head[u] == head[v]) return u;
            v = par[head[v]];
        }
    }

    int dist(int s, int t) const {
        int p = lca(s, t);
        return dep[s] + dep[t] - 2 * dep[p];
    }

    // 0 <= k <= dist(s, t) でない場合-1
    int jump_on_tree(int s, int t, int k) const {
        int p = lca(s, t);
        int sp = dep[s] - dep[p];
        if (k <= sp) {
            return la(s, k);
        } else {
            int pt = dep[t] - dep[p];
            if (k > sp + pt) return -1;
            return la(t, sp + pt - k);
        }
    }

    // pの部分木がcを含むか
    bool contain_subtree(int p, int c) const {
        if (dep[p] > dep[c]) return false;
        return p == la(c, dep[c] - dep[p]);
    }

    // s->tパス(端点も含む)がvを含むか
    bool contain_path(int s, int t, int v) const {
        if (lca(s, t) == v) return true;
        return contain_subtree(v, s) ^ contain_subtree(v, t);
    }

    int ord_point(int v) const {
        return in[v];
    }

    // [l, r)
    std::pair<int, int> ord_subtree(int v) const {
        return {in[v], out[v]};
    }

    // O(logN)個の区間でパスを表す
    // 各区間はl < rまたはl > rを満たし
    // l < rなら上から下に降りる順番
    // l > rなら下から上に登る順番
    // (s -> t パスの順序を保っている)
    std::vector<std::pair<int, int>> ord_path(int s, int t) const {
        std::vector<std::pair<int, int>> up, down;
        while (head[s] != head[t]) {
            if (in[s] < in[t]) {
                down.push_back({in[head[t]], in[t] + 1});
                t = par[head[t]];
            } else {
                up.push_back({in[s] + 1, in[head[s]]});
                s = par[head[s]];
            }
        }
        if (in[s] < in[t]) {
            down.push_back({in[s], in[t] + 1});
        } else {
            up.push_back({in[s] + 1, in[t]});
        }
        std::reverse(down.begin(), down.end());
        up.insert(up.end(), down.begin(), down.end());
        return up;
    }

    // l < rなら上から下に降りる順番
    // l > rなら下から上に登る順番
    // (s -> t パスの順序を保っていない, 区間と向きは合っている)
    template<typename F>
    void query_path(int s, int t, F f) {
        while (head[s] != head[t]) {
            if (in[s] < in[t]) {
                f(in[head[t]], in[t] + 1);
                t = par[head[t]];
            } else {
                f(in[s] + 1, in[head[s]]);
                s = par[head[s]];
            }
        }
        if (in[s] < in[t]) {
            f(in[s], in[t] + 1);
        } else {
            f(in[s] + 1, in[t]);
        }
    }
    
    // F := [l, r)のモノイド積を得る関数
    template<typename S, S (*op)(S, S), S (*e)(), S (*flip)(S), typename F>
    S prod_path(int s, int t, F f) {
        S sl = e(), sr = e();
        query_path(s, t, [&](int l, int r) {
            if (l > r) {
                sl = op(sl, flip(f(r, l)));
            } else {
                sr = op(f(l, r), sr);
            }
        });
        return op(sl, sr);
    }
    
    // path [a,b] と [c,d] の交わり. 空ならば {-1,-1}.
    std::pair<int, int> intersection_path(int a, int b, int c, int d) const{
        int ab = lca(a, b), ac = lca(a, c), ad = lca(a, d);
        int bc = lca(b, c), bd = lca(b, d), cd = lca(c, d);
        int x = ab ^ ac ^ bc, y = ab ^ ad ^ bd;
        if (x != y) return {x, y};
        int z = ac ^ ad ^ cd;
        if (x != z) x = -1;
        return {x, x};
    }

    // {頂点集合(in順), 辺集合}
    std::pair<std::vector<int>, std::vector<std::pair<int, int>>> auxiliary_tree(std::vector<int> v) const {
        if (v.empty()) return {{}, {}};
        std::sort(v.begin(), v.end(), [&](int a, int b) {return in[a] < in[b]; });
        std::vector<int> V;
        std::vector<std::pair<int, int>> E;
        std::vector<int> st;
        st.push_back(v[0]);
        for (int i = 1; i < v.size(); i++) {
            if (v[i] == v[i - 1]) continue;
            int l = lca(v[i], v[i - 1]);
            while (true) {
                int c = st.back();
                st.pop_back();
                if (st.empty() || dep[st.back()] < dep[l]) {
                    st.push_back(l);
                    st.push_back(v[i]);
                    if (dep[c] > dep[l]) {
                        E.push_back({l, c});
                        V.push_back(c);
                    }
                    break;
                }
                E.push_back({st.back(), c});
                V.push_back(c);
            }
        }
        while (st.size() >= 2) {
            int c = st.back();
            st.pop_back();
            int p = st.back();
            E.push_back({p, c});
            V.push_back(c);
        }
        V.push_back(st[0]);
        return {V, E};
    }
};
#endif