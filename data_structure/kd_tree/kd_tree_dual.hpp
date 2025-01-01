#ifndef _KD_TREE_DUAL_H_
#define _KD_TREE_DUAL_H_
#include <numeric>
#include <vector>
#include <algorithm>
#include <cassert>

template<typename Idx, typename S, S (*op)(S, S), S (*e)()>
struct kd_tree_dual {
  public:
    struct point {
        Idx x, y;
        S z;
        point() {}
        point(Idx x, Idx y, S z): x(x), y(y), z(z) {}
    };

  private:
    static constexpr Idx inf = std::numeric_limits<Idx>::max() / 2;
    static constexpr Idx minf = -inf;

    struct node {
        Idx lx, rx, ly, ry;
        int r;
        S sum;
        node () : lx(inf), rx(minf), ly(inf), ry(minf), r(0), sum(e()) {}
        node (point p) : lx(p.x), rx(p.x), ly(p.y), ry(p.y), r(0), sum(p.z) {}
    };
  
    std::vector<node> V{node()};
    std::vector<int> Leaf, Par{0}, Flag{0};

    void update_coord(int v) {
        V[v].lx = V[v].ly = inf;
        V[v].rx = V[v].ry = minf;
        if (Flag[v + 1] > 0) {
            node &u = V[v + 1];
            V[v].lx = u.lx;
            V[v].rx = u.rx;
            V[v].ly = u.ly;
            V[v].ry = u.ry;
        }
        if (Flag[V[v].r] > 0) {
            node &u = V[V[v].r];
            V[v].lx = std::min(V[v].lx, u.lx);
            V[v].rx = std::max(V[v].rx, u.rx);
            V[v].ly = std::min(V[v].ly, u.ly);
            V[v].ry = std::max(V[v].ry, u.ry);
        }
    }

    int make_node() {
        int res = V.size();
        V.push_back(node());
        Par.push_back(0);
        Flag.push_back(2);
        return res;
    }

    int make_node(point p) {
        int res = V.size();
        V.push_back(node(p));
        Par.push_back(0);
        Flag.push_back(1);
        return res;
    }

    int build(std::vector<std::pair<point, int>> &v, int l, int r, bool dim) {
        if(r - l == 1) {
            int u = make_node(v[l].first);
            Leaf[v[l].second] = u;
            return u;
        }
        int m = (l + r) / 2;
        if constexpr (true) {
            if (!dim) {
                std::nth_element(v.begin() + l, v.begin() + m, v.begin() + r, [](const auto &a, const auto &b) { return a.first.x < b.first.x; });
            } else {
                std::nth_element(v.begin() + l, v.begin() + m, v.begin() + r, [](const auto &a, const auto &b) { return a.first.y < b.first.y; });
            }
        } else {
            // ヒューリスティック
            Idx lx = inf, rx = minf, ly = inf, ry = minf;
            for (int i = l; i < r; i++) {
                lx = std::min(lx, v[i].first.x);
                rx = std::max(rx, v[i].first.x);
                ly = std::min(ly, v[i].first.y);
                ry = std::max(ry, v[i].first.y);
            }
            if ((long long)rx - lx > (long long)ry - ly) {
                std::nth_element(v.begin() + l, v.begin() + m, v.begin() + r, [](const auto &a, const auto &b) { return a.first.x < b.first.x; });
            } else {
                std::nth_element(v.begin() + l, v.begin() + m, v.begin() + r, [](const auto &a, const auto &b) { return a.first.y < b.first.y; });
            }
        }
        int u = make_node();
        build(v, l, m, !dim);
        Par[u + 1] = u;
        V[u].r = build(v, m, r, !dim);
        Par[V[u].r] = u;
        update_coord(u);
        return u;
    }

    void push(int v) {
        if (V[v].sum != e()) {
            all_apply(v + 1, V[v].sum);
            all_apply(V[v].r, V[v].sum);
            V[v].sum = e();
        }
    }

    void all_apply(int v, S lz) {
        if (Flag[v] == 0) return;
        V[v].sum = op(V[v].sum, lz);
    }

    int root = 1;
    static std::array<int, 100000> st;

  public:
    kd_tree_dual() {}
    kd_tree_dual(const std::vector<point> &v) : Leaf(v.size()) { 
        if(!v.empty()) {
            std::vector<std::pair<point, int>> P(v.size());
            for (int i = 0; i < v.size(); i++) P[i] = {v[i], i};
            root = build(P, 0, v.size(), 0);
        }
    }
    
    // offにした点番号iを復活
    void on(int i) {
        if (Flag[i] == 1) return;
        auto f = [&](auto &&f, int j) -> void {
            if (Par[j]) f(f, Par[j]);
            if (Flag[j] == 2) push(j);
        };
        i = Leaf[i];
        f(f, i);
        Flag[i] = 1;
        while (i != root) {
            i = Par[i];
            update_coord(i);
        }
    }

    // 点番号iを一時的に削除
    void off(int i) {
        if (Flag[i] == 0) return;
        auto f = [&](auto &&f, int j) -> void {
            if (Par[j]) f(f, Par[j]);
            if (Flag[j] == 2) push(j);
        };
        i = Leaf[i];
        f(f, i);
        Flag[i] = 0;
        while (i != root) {
            i = Par[i];
            update_coord(i);
        }
    }

    // 点番号iの値をzにする
    void set(int i, S z) {
        auto f = [&](auto &&f, int j) -> void {
            if (Par[j]) f(f, Par[j]);
            if (Flag[j] == 2) push(j);
        };
        i = Leaf[i];
        f(f, i);
        V[i].sum = z;
    }
    
    // 点番号iの値
    point get(int i) {
        auto f = [&](auto &&f, int j) -> void {
            if (Par[j]) f(f, Par[j]);
            if (Flag[j] == 2) push(j);
        };
        i = Leaf[i];
        f(f, i);
        return {V[i].lx, V[i].ly, V[i].sum};
    }
    // O(√N)
    void apply_rectangle(Idx LX, Idx RX, Idx LY, Idx RY, S lz) {
        RX--, RY--;
        if (LX > RX || LY > RY) return;
        if (root >= V.size()) return;
        int pos = 0;
        st[0] = root;
        while(pos >= 0) {
            int v = st[pos--];
            if (V[v].rx < LX || RX < V[v].lx || V[v].ry < LY || RY < V[v].ly) continue;
            if (LX <= V[v].lx && V[v].rx <= RX && LY <= V[v].ly && V[v].ry <= RY) {
                all_apply(v, lz);
            } else {
                push(v);
                st[++pos] = V[v].r;
                st[++pos] = v + 1;
            } 
        }
    }

    template<typename Idx2>
    void apply_euclid(Idx x, Idx y, Idx d, S lz) {
        if (root >= V.size()) return e();
        int pos = 0;
        st[0] = root;
        while(pos >= 0) {
            int v = st[pos--];
            Idx dxmin = (x < V[v].lx ? V[v].lx - x : V[v].rx < x ? x - V[v].rx : 0);
            Idx dymin = (y < V[v].ly ? V[v].ly - y : V[v].ry < y ? y - V[v].ry : 0);
            if ((Idx2)dxmin * dxmin + (Idx2)dymin * dymin > (Idx2)d * d) continue;
            Idx dxmax = std::max(abs(V[v].rx - x), abs(V[v].lx - x));
            Idx dymax = std::max(abs(V[v].ry - y), abs(V[v].ly - y));
            if ((Idx2)dxmax * dxmax + (Idx2)dymax * dymax <= (Idx2)d * d) {
                all_apply(v, lz);
            } else {
                push(v);
                st[++pos] = V[v].r;
                st[++pos] = v + 1;
            } 
        }
    }

    void apply_manhattan(Idx x, Idx y, Idx d, S lz) {
        if (root >= V.size()) return e();
        int pos = 0;
        st[0] = root;
        while(pos >= 0) {
            int v = st[pos--];
            Idx dxmin = (x < V[v].lx ? V[v].lx - x : V[v].rx < x ? x - V[v].rx : 0);
            Idx dymin = (y < V[v].ly ? V[v].ly - y : V[v].ry < y ? y - V[v].ry : 0);
            if (dxmin + dymin > d) continue;
            Idx dxmax = std::max(abs(V[v].rx - x), abs(V[v].lx - x));
            Idx dymax = std::max(abs(V[v].ry - y), abs(V[v].ly - y));
            if (dxmax + dymax <= d) {
                all_apply(v, lz);
            } else {
                push(v);
                st[++pos] = V[v].r;
                st[++pos] = v + 1;
            } 
        }
    }

    // O(√N + (含まれる点の個数))
    template<typename G>
    void enumerate_rectangle(Idx LX, Idx RX, Idx LY, Idx RY, G g) {
        RX--, RY--;
        if (LX > RX || LY > RY) return;
        if (root >= V.size()) return;
        int pos = 0;
        st[0] = root;
        while(pos >= 0) {
            int v = st[pos--];
            if (V[v].rx < LX || RX < V[v].lx || V[v].ry < LY || RY < V[v].ly) continue;
            if (Flag[v] < 2) {
                if (Flag[v] == 1) {
                    g(V[v].lx, V[v].ly, V[v].sum);
                }
            } else {
                push(v);
                st[++pos] = V[v].r;
                st[++pos] = v + 1;
            } 
        }
    }

    template<typename Idx2, typename G>
    void enumerate_euclid(Idx x, Idx y, Idx d, G g) {
        if (root >= V.size()) return;
        int pos = 0;
        st[0] = root;
        while(pos >= 0) {
            int v = st[pos--];
            Idx dxmin = (x < V[v].lx ? V[v].lx - x : V[v].rx < x ? x - V[v].rx : 0);
            Idx dymin = (y < V[v].ly ? V[v].ly - y : V[v].ry < y ? y - V[v].ry : 0);
            if ((Idx2)dxmin * dxmin + (Idx2)dymin * dymin > (Idx2)d * d) continue;
            if (Flag[v] < 2) {
                if (Flag[v] == 1) {
                    g(V[v].lx, V[v].ly, V[v].sum);
                }
            } else {
                push(v);
                st[++pos] = V[v].r;
                st[++pos] = v + 1;
            } 
        }
    }

    template<typename G>
    void enumerate_manhattan(Idx x, Idx y, Idx d, G g) {
        if (root >= V.size()) return;
        int pos = 0;
        st[0] = root;
        while(pos >= 0) {
            int v = st[pos--];
            Idx dxmin = (x < V[v].lx ? V[v].lx - x : V[v].rx < x ? x - V[v].rx : 0);
            Idx dymin = (y < V[v].ly ? V[v].ly - y : V[v].ry < y ? y - V[v].ry : 0);
            if (dxmin + dymin > d) continue;
            if (Flag[v] < 2) {
                if (Flag[v] == 1) {
                    g(V[v].lx, V[v].ly, V[v].sum);
                }
            } else {
                push(v);
                st[++pos] = V[v].r;
                st[++pos] = v + 1;
            } 
        }
    }
};

template<typename Idx, typename S, S (*op)(S, S), S (*e)()>
std::array<int, 100000> kd_tree_dual<Idx, S, op, e>::st;
#endif