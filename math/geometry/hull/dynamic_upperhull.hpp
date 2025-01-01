#ifndef _DYNAMIC_UPPERHULL_H_
#define _DYNAMIC_UPPERHULL_H_
#include <vector>
#include <algorithm>
#include <numeric>
#include <cassert>

// T : 2 * 座標の最大値が収まる型
// T2 : (2 * 座標の最大値)^2 が収まる型
// T3 : (2 * 座標の最大値)^3 が収まる型
template <typename T, typename T2, typename T3>
struct dynamic_upperhull {
    struct point {
        T x, y;
        bool operator == (const point &p) const { return x == p.x && y == p.y; }
        bool operator != (const point &p) const { return x != p.x || y != p.y; }
        bool operator < (const point &p) const { return x < p.x || (x == p.x && y < p.y); }
        bool operator <= (const point &p) const { return x < p.x || (x == p.x && y <= p.y); }
        static T2 cross(const point &a, const point &b, const point &c) {
            return (T2)(b.y - a.y) * (c.x - a.x) - (T2)(c.y - a.y) * (b.x - a.x);
        }
    };

    struct node {
        int lch, rch;
        point lmost, lbr, rbr;
        node(){}
        node(T x, T y): lmost{x, y}, lbr{x, y}, rbr{x, y} {}
    };

  public:
    static constexpr T inf = std::numeric_limits<T>::max();
    static constexpr T minf = std::numeric_limits<T>::min();

    dynamic_upperhull() {}
    dynamic_upperhull(const std::vector<std::pair<T, T>>& v) : _n(int(v.size())), cnt(_n), root(0) {
        if(v.empty()) return;
        size = _n - 1;
        nd = std::vector<node>(size + _n);
        correct = std::vector<bool>(size + _n, true);
        for (int i = 0; i < _n; i++) nd[size + i] = node(v[i].first, v[i].second);
        int u = 0;
        auto dfs = [&] (auto &&dfs, int l, int r) -> int {
            if (r - l == 1) return l + size;
            int m = (l + r) / 2;
            int res = u++;
            nd[res].lch = dfs(dfs, l, m);
            nd[res].rch = dfs(dfs, m, r);
            pull_lmost(res);
            pull(res);
            return res;
        };
        root = dfs(dfs, 0, _n);
    }

    // 点の数
    int count() { return cnt; }
    
    void erase(int id) {
        assert(cnt);
        assert(0 <= id && id < _n);
        id += size;
    
        std::vector<int> V;
        point &p = nd[id].lmost;
        if (p.x == inf) return; // すでに削除済

        cnt--;
        if (cnt == 0) {
            p.x = inf;
            return;
        }
        int v = root;
        while (v < size) {
            V.push_back(v);
            if (p < nd[nd[v].rch].lmost) {
                v = nd[v].lch;
            } else {
                v = nd[v].rch;
            }
        }
        assert(V.size() >= 1);
        if(V.size() == 1) {
            p.x = inf;
            if (nd[root].lch == id) {
                root = nd[root].rch;
            } else {
                root = nd[root].lch;
            }
            return;
        }
        int vp = V.back();
        V.pop_back();
        int vpp = V.back();
        int u = (nd[vp].lch == v ? nd[vp].rch : nd[vp].lch);
        if (nd[vpp].lch == vp) {
            nd[vpp].lch = u;
        } else {
            nd[vpp].rch = u;
        }
        while (!V.empty()) {
            vp = V.back();
            V.pop_back();
            pull_lmost(vp);
            if (nd[vp].lbr == p || nd[vp].rbr == p) correct[vp] = false;
        }
        p.x = inf;
    }

    std::vector<int> get_hull() {
        if (!cnt) return {};
        std::vector<int> res;
        _get_hull(root, {minf, minf}, {inf, inf}, res);
        return res;
    }
    
    // 番号idの点のy座標を変更
    void change_y(int id, T y){
        assert(0 <= id && id < _n);
        id += size;
        std::vector<int> V;
        point p = nd[id].lmost;
        if (p.x == inf) return; // すでに削除済
        int v = root;
        while (v < size) {
            V.push_back(v);
            if (p < nd[nd[v].rch].lmost) {
                v = nd[v].lch;
            } else {
                v = nd[v].rch;
            }
        }
        nd[id].lmost.y = y;
        nd[id].lbr.y = y;
        nd[id].rbr.y = y;
        while (!V.empty()) {
            int vp = V.back();
            V.pop_back();
            pull_lmost(vp);
            if (p.y < y || nd[vp].lbr == p || nd[vp].rbr == p) {
                correct[vp] = false;
            }
        }
    }

    // 全ての点が(a, b)より大きい時に使える
    // 点(a, b)との角度が最も大きい点番号を返す
    int max_slope(T a, T b){
        assert(cnt);
        {
            point tmp{a, b};
            assert(tmp < nd[root].lmost);
        }
        int v = root;
        while (v < size) {
            if (!correct[v]) pull(v);
            T lx = nd[v].lbr.x - a, ly = nd[v].lbr.y - b;
            T rx = nd[v].rbr.x - a, ry = nd[v].rbr.y - b;
            if ((T2)ly * rx <= (T2)lx * ry) {
                v = nd[v].rch;
            } else {
                v = nd[v].lch;
            }
        }
        return v - size;
    }

    // 上側凸包上の点のうちp以上の最小の点の番号, ない場合は-1
    int lower_bound(point p) {
        int v = root;
        point q{inf, inf};
        while (v < size) {
            if (!correct[v]) pull(v);
            if (nd[v].rbr < p) {
                v = nd[v].rch;
            } else if (p <= nd[v].lbr) {
                q = std::min(q, nd[v].lbr);
                v = nd[v].lch;
            } else if(q < nd[v].rbr) {
                v = nd[v].lch;
            } else {
                assert(nd[v].lbr < p && p <= nd[v].rbr && nd[v].rbr <= q);
                p = q = nd[v].rbr;
                v = nd[v].rch;
            }
        }
        if (q.x == inf) return -1;
        return v - size;
    }

    // 上側凸包上の点のうちp以下の最大の点の番号, ない場合は-1
    int lower_bound_rev(point p) {
        int v = root;
        point q{minf, minf};
        while (v < size) {
            if (!correct[v]) pull(v);
            if (p < nd[v].lbr) {
                v = nd[v].lch;
            } else if (nd[v].rbr <= p) {
                q = std::max(q, nd[v].rbr);
                v = nd[v].rch;
            } else if(nd[v].lbr < q) {
                v = nd[v].rch;
            } else {
                assert(q <= nd[v].lbr && nd[v].lbr <= p && p < nd[v].rbr);
                p = q = nd[v].lbr;
                v = nd[v].lch;
            }
        }
        if (q.x == minf) return -1;
        return v - size;
    }
    
  private:
    int _n, size, cnt, root;
    std::vector<node> nd;
    std::vector<bool> correct;

    void pull_lmost(int k) {
        nd[k].lmost = nd[nd[k].lch].lmost;
    }

    void pull(int k) {
        assert(k < size);
        correct[k] = true;
        int l = nd[k].lch, r = nd[k].rch;
        if (!correct[l]) pull(l);
        if (!correct[r]) pull(r);
        T splitx = nd[r].lmost.x;
        point a = nd[l].lbr, b = nd[l].rbr, c = nd[r].lbr, d = nd[r].rbr;

        #define movel(f){\
            l = (f ? nd[l].rch : nd[l].lch);\
            if (!correct[l]) pull(l);\
            a = nd[l].lbr, b = nd[l].rbr;\
        }
        #define mover(f){\
            r = (f ? nd[r].rch : nd[r].lch);\
            if (!correct[r]) pull(r);\
            c = nd[r].lbr, d = nd[r].rbr;\
        }
        while ((l < size) || (r < size)) {
            T3 s1 = point::cross(a, b, c);
            if (l < size && s1 < 0) {
                movel(0);
            } else if (r < size && point::cross(b, c, d) < 0) {
                mover(1);
            } else if (l >= size) {
                mover(0);
            } else if (r >= size) {
                movel(1);
            } else {
                T3 s2 = point::cross(b, a, d);
                assert(s1 + s2 <= 0);
                if (s1 + s2 == 0 || s1 * (d.x - splitx) > s2 * (splitx - c.x)) {
                    movel(1);
                } else {
                    mover(0);
                }
            }
        }
        nd[k].lbr = a;
        nd[k].rbr = c;
        #undef movel
        #undef mover
    }

    void _get_hull(int v, point L, point R, std::vector<int> &res) {
        if (v >= size) {
            res.push_back(v - size);
            return;
        }
        if (!correct[v]) pull(v);
        if (R <= nd[v].lbr) {
            _get_hull(nd[v].lch, L, R, res);
        } else if (nd[v].rbr <= L) {
            _get_hull(nd[v].rch, L, R, res);
        } else {
            _get_hull(nd[v].lch, L, nd[v].lbr, res);
            _get_hull(nd[v].rch, nd[v].rbr, R, res);
        }
    }
};
#endif