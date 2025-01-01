#ifndef _KD_TREE_H_
#define _KD_TREE_H_
#include <numeric>
#include <vector>
#include <algorithm>
#include <cassert>

template<typename Idx, typename S, S (*op)(S, S), S (*e)()>
struct kd_tree {
  public:
    struct point {
        Idx x, y;
        S z;
        point() {}
        point(Idx x, Idx y, S z) : x(x), y(y), z(z) {}
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

    void update_val(int v) {
        S lsum = Flag[v + 1] > 0 ? V[v + 1].sum : e();
        S rsum = Flag[V[v].r] > 0 ? V[V[v].r].sum : e();
        V[v].sum = op(lsum, rsum);
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
        update_val(u);
        update_coord(u);
        return u;
    }
    int root = 1;
    static std::array<int, 100000> st;

  public:
    kd_tree() {}
    kd_tree(const std::vector<point> &v) : Leaf(v.size()) { 
        if(!v.empty()) {
            std::vector<std::pair<point, int>> P(v.size());
            for (int i = 0; i < v.size(); i++) P[i] = {v[i], i};
            root = build(P, 0, v.size(), 0);
        }
    }

    // offにした点番号iを復活
    void on(int i) {
        i = Leaf[i];
        if (Flag[i] == 1) return;
        Flag[i] = 1;
        while (i) {
            i = Par[i];
            update_val(i);
            update_coord(i);
        }
    }

    // 点番号iを一時的に削除
    void off(int i) {
        i = Leaf[i];
        if (Flag[i] == 0) return;
        Flag[i] = 0;
        while (i) {
            i = Par[i];
            update_val(i);
            update_coord(i);
        }
    }

    // 点番号iの値をzにする
    void set(int i, S z) {
        i = Leaf[i];
        V[i].sum = z;
        while (i) {
            i = Par[i];
            update_val(i);
        }
    }

    // 点番号iの値
    point get(int i) {
        i = Leaf[i];
        return {V[i].lx, V[i].ly, V[i].sum};
    }

    // O(√N)
    S prod_rectangle(Idx LX, Idx RX, Idx LY, Idx RY) {
        RX--, RY--;
        if (LX > RX || LY > RY) return e();
        if (root >= V.size()) return e();
        int pos = 0;
        st[0] = root;
        S res = e();
        while(pos >= 0) {
            int v = st[pos--];
            if (V[v].rx < LX || RX < V[v].lx || V[v].ry < LY || RY < V[v].ly) continue;
            if (LX <= V[v].lx && V[v].rx <= RX && LY <= V[v].ly && V[v].ry <= RY) {
                res = op(res, V[v].sum);
            } else {
                st[++pos] = V[v].r;
                st[++pos] = v + 1;
            } 
        }
        return res;
    }

    template<typename Idx2>
    S prod_euclid(Idx x, Idx y, Idx d) {
        if (root >= V.size()) return e();
        int pos = 0;
        st[0] = root;
        S res = e();
        while(pos >= 0) {
            int v = st[pos--];
            Idx dxmin = (x < V[v].lx ? V[v].lx - x : V[v].rx < x ? x - V[v].rx : 0);
            Idx dymin = (y < V[v].ly ? V[v].ly - y : V[v].ry < y ? y - V[v].ry : 0);
            if ((Idx2)dxmin * dxmin + (Idx2)dymin * dymin > (Idx2)d * d) continue;
            Idx dxmax = std::max(abs(V[v].rx - x), abs(V[v].lx - x));
            Idx dymax = std::max(abs(V[v].ry - y), abs(V[v].ly - y));
            if ((Idx2)dxmax * dxmax + (Idx2)dymax * dymax <= (Idx2)d * d) {
                res = op(res, V[v].sum);
            } else {
                st[++pos] = V[v].r;
                st[++pos] = v + 1;
            } 
        }
        return res;
    }

    S prod_manhattan(Idx x, Idx y, Idx d) {
        if (root >= V.size()) return e();
        int pos = 0;
        st[0] = root;
        S res = e();
        while(pos >= 0) {
            int v = st[pos--];
            Idx dxmin = (x < V[v].lx ? V[v].lx - x : V[v].rx < x ? x - V[v].rx : 0);
            Idx dymin = (y < V[v].ly ? V[v].ly - y : V[v].ry < y ? y - V[v].ry : 0);
            if (dxmin + dymin > d) continue;
            Idx dxmax = std::max(abs(V[v].rx - x), abs(V[v].lx - x));
            Idx dymax = std::max(abs(V[v].ry - y), abs(V[v].ly - y));
            if (dxmax + dymax <= d) {
                res = op(res, V[v].sum);
            } else {
                st[++pos] = V[v].r;
                st[++pos] = v + 1;
            } 
        }
        return res;
    }

    // 線分abの下
    // judge_y 線上を含むかどうか
    // judge_l 左端を含むかどうか
    // judge_r 右端を含むかどうか
    // std::greater : 含まない
    // std::greater_equal : 含む
    template<typename Idx2, typename judge_y, typename judge_l, typename judge_r>
    S prod_under_segment(point a, point b) {
        if (root >= V.size()) return e();
        if (a.x > b.x) std::swap(a, b);
        auto cross = [a, b](Idx x, Idx y) -> Idx2 {
            return Idx2(b.x - a.x) * (y - a.y) - Idx2(b.y - a.y) * (x - a.x);
        };
        int pos = 0;
        st[0] = root;
        S res = e();
        while(pos >= 0) {
            int v = st[pos--];
            if (!judge_l()(V[v].rx, a.x) || !judge_r()(b.x, V[v].lx)) continue;
            Idx lx = std::max(V[v].lx, a.x);
            Idx rx = std::min(V[v].rx, b.x);
            if (!judge_y()(0, cross((a.y < b.y ? rx : lx), V[v].ly))) continue;
            if (judge_l()(V[v].lx, a.x) && judge_r()(b.x, V[v].rx) && judge_y()(0, cross((a.y < b.y ? lx : rx), V[v].ry))) {
                res = op(res, V[v].sum);
            } else {
                st[++pos] = V[v].r;
                st[++pos] = v + 1;
            }
        }
        return res;
    }

    // O(√N + (含まれる点の個数))
    template<typename F>
    void enumerate_rectangle(Idx LX, Idx RX, Idx LY, Idx RY, F f) {
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
                    f(V[v].lx, V[v].ly, V[v].sum);
                }
            } else {
                st[++pos] = V[v].r;
                st[++pos] = v + 1;
            } 
        }
    }

    template<typename Idx2, typename F>
    void enumerate_euclid(Idx x, Idx y, Idx d, F f) {
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
                    f(V[v].lx, V[v].ly, V[v].sum);
                }
            } else {
                st[++pos] = V[v].r;
                st[++pos] = v + 1;
            } 
        }
    }

    template<typename F>
    void enumerate_manhattan(Idx x, Idx y, Idx d, F f) {
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
                    f(V[v].lx, V[v].ly, V[v].sum);
                }
            } else {
                st[++pos] = V[v].r;
                st[++pos] = v + 1;
            } 
        }
    }

    // onの点のうち(x, y)と最も近い点 
    template<typename Idx2>
    point closest_euclid(Idx x, Idx y) {
        Idx2 mindist = std::numeric_limits<Idx2>::max();
        point res;
        auto dfs = [&](auto &&dfs, int v) -> void {
            if (Flag[v] < 2) {
                Idx2 dx = V[v].lx - x, dy = V[v].ly - y;
                Idx2 dist = dx * dx + dy * dy;
                if (Flag[v] == 1 && dist < mindist) {
                    mindist = dist;
                    res = {V[v].lx, V[v].ly, V[v].sum};
                }
                return;
            }
            int a = v + 1, b = V[v].r;
            Idx2 dx1 = std::max({x - V[a].rx, V[a].lx - x, 0}), dy1 = std::max({y - V[a].ry, V[a].ly - y, 0});
            Idx2 dx2 = std::max({x - V[b].rx, V[b].lx - x, 0}), dy2 = std::max({y - V[b].ry, V[b].ly - y, 0});
            Idx2 dist1 = dx1 * dx1 + dy1 * dy1, dist2 = dx2 * dx2 + dy2 * dy2;
            if (dist1 < dist2) {
                if (mindist > dist1) dfs(dfs, a);
                if (mindist > dist2) dfs(dfs, b);
            } else {
                if (mindist > dist2) dfs(dfs, b);
                if (mindist > dist1) dfs(dfs, a);
            }
        };
        dfs(dfs, root);
        return res;
    }

    // onの点のうち(x, y)と最も遠い点
    template<typename Idx2>
    point farthest_euclid(Idx x, Idx y) {
        Idx2 maxdist = std::numeric_limits<Idx2>::min();
        point res;
        auto dfs = [&](auto &&dfs, int v) -> void {
            if (Flag[v] < 2) {
                Idx2 dx = V[v].lx - x, dy = V[v].ly - y;
                Idx2 dist = dx * dx + dy * dy;
                if (Flag[v] == 1 && dist > maxdist) {
                    maxdist = dist;
                    res = {V[v].lx, V[v].ly, V[v].sum};
                }
                return;
            }
            int a = v + 1, b = V[v].r;
            Idx2 dx1 = std::max(x - V[a].lx, V[a].rx - x), dy1 = std::max(y - V[a].ly, V[a].ry - y);
            Idx2 dx2 = std::max(x - V[b].lx, V[b].rx - x), dy2 = std::max(y - V[b].ly, V[b].ry - y);
            Idx2 dist1 = dx1 * dx1 + dy1 * dy1, dist2 = dx2 * dx2 + dy2 * dy2;
            if (dist1 > dist2) {
                if (maxdist < dist1) dfs(dfs, a);
                if (maxdist < dist2) dfs(dfs, b);
            } else {
                if (maxdist < dist2) dfs(dfs, b);
                if (maxdist < dist1) dfs(dfs, a);
            }
        };
        dfs(dfs, root);
        return res;
    }

    // onの点のうち(x, y)と最も近い点 
    point closest_manhattan(Idx x, Idx y) {
        Idx mindist = std::numeric_limits<Idx>::max();
        point res;
        auto dfs = [&](auto &&dfs, int v) -> void {
            if (Flag[v] < 2) {
                Idx dist = abs(V[v].lx - x) + abs(V[v].ly - y);
                if (Flag[v] == 1 && dist < mindist) {
                    mindist = dist;
                    res = {V[v].lx, V[v].ly, V[v].sum};
                }
                return;
            }
            int a = v + 1, b = V[v].r;
            Idx dx1 = std::max({x - V[a].rx, V[a].lx - x, 0}), dy1 = std::max({y - V[a].ry, V[a].ly - y, 0});
            Idx dx2 = std::max({x - V[b].rx, V[b].lx - x, 0}), dy2 = std::max({y - V[b].ry, V[b].ly - y, 0});
            if (dx1 + dy1 < dx2 + dy2) {
                if (mindist > dx1 + dy1) dfs(dfs, a);
                if (mindist > dx2 + dy2) dfs(dfs, b);
            } else {
                if (mindist > dx2 + dy2) dfs(dfs, b);
                if (mindist > dx1 + dy1) dfs(dfs, a);
            }
        };
        dfs(dfs, root);
        return res;
    }

    // onの点のうち(x, y)と最も遠い点
    point farthest_manhattan(Idx x, Idx y) {
        Idx maxdist = std::numeric_limits<Idx>::min();
        point res;
        auto dfs = [&](auto &&dfs, int v) -> void {
            if (Flag[v] < 2) {
                Idx dist = abs(V[v].lx - x) + abs(V[v].ly - y);
                if (Flag[v] == 1 && dist > maxdist) {
                    maxdist = dist;
                    res = {V[v].lx, V[v].ly, V[v].sum};
                }
                return;
            }
            int a = v + 1, b = V[v].r;
            Idx dx1 = std::max(x - V[a].lx, V[a].rx - x), dy1 = std::max(y - V[a].ly, V[a].ry - y);
            Idx dx2 = std::max(x - V[b].lx, V[b].rx - x), dy2 = std::max(y - V[b].ly, V[b].ry - y);
            if (dx1 + dy1 > dx2 + dy2) {
                if (maxdist < dx1 + dy1) dfs(dfs, a);
                if (maxdist < dx2 + dy2) dfs(dfs, b);
            } else {
                if (maxdist < dx2 + dy2) dfs(dfs, b);
                if (maxdist < dx1 + dy1) dfs(dfs, a);
            }
        };
        dfs(dfs, root);
        return res;
    }
};

template<typename Idx, typename S, S (*op)(S, S), S (*e)()>
std::array<int, 100000> kd_tree<Idx, S, op, e>::st;
#endif