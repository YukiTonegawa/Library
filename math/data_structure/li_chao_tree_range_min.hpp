#ifndef _LI_CHAO_TREE_RANGE_MIN_H_
#define _LI_CHAO_TREE_RANGE_MIN_H_
#include <algorithm>
#include <vector>

template<typename T, T xmin, T xmax>
struct li_chao_tree_range_min {
    static constexpr T inf = std::numeric_limits<T>::max();
    struct line {
        T a, b;
        line(T a, T b): a(a), b(b){}
        inline T get(T x){return a * x + b;}
        bool operator != (const line &r){return a != r.a || b != r.b;}
    };
  private:
    struct node {
        line x;
        node *l, *r;
        T min;
        node(const line &x, T m) : x(x), l(nullptr), r(nullptr), min(m){}
    };
    node *root;
    node *add_line(node *v, line &x, T l, T r, T x_l, T x_r) {
        if (!v) return new node(x, std::min(x_l, x_r));
        T t_l = v->x.get(l), t_r = v->x.get(r);
        if (l + 1 == r) {
            if (x_l < t_l) v->x = x, v->min = x_l;
            return v;
        } else if (t_l <= x_l && t_r <= x_r) {
            return v;
        } else if (t_l >= x_l && t_r >= x_r) {
            v->x = x;
            v->min = std::min(v->min, std::min(x_l, x_r));
            return v;
        } else {
            T m = (l + r) / 2;
            if (m == r) --m;
            T t_m = v->x.get(m), x_m = x.get(m);
            if (t_m > x_m) {
                std::swap(v->x, x);
                v->min = std::min(v->min, std::min(x_l, x_r));
                if (x_l >= t_l) v->l = add_line(v->l, x, l, m, t_l, t_m);
                else v->r = add_line(v->r, x, m + 1, r, t_m + x.a, t_r);
            } else {
                if (t_l >= x_l) v->l = add_line(v->l, x, l, m, x_l, x_m);
                else v->r = add_line(v->r, x, m + 1, r, x_m + x.a, x_r);
            }
            if (v->l && v->min > v->l->min) v->min = v->l->min;
            if (v->r && v->min > v->r->min) v->min = v->r->min;
            return v;
        }
    }
    node *add_segment(node *v, line &x, T a, T b, T l, T r, T x_l, T x_r) {
        if (r < a || b < l) return v;
        if (a <= l && r <= b) {
            line y(x);
            return add_line(v, y, l, r, x_l, x_r);
        }
        if (v) {
            T t_l = v->x.get(l), t_r = v->x.get(r);
            if (t_l <= x_l && t_r <= x_r) return v;
        } else {
            v = new node(line(0, inf), inf);
        }
        T m = (l + r) / 2;
        if (m == r) --m;
        T x_m = x.get(m);
        v->l = add_segment(v->l, x, a, b, l, m, x_l, x_m);
        v->r = add_segment(v->r, x, a, b, m + 1, r, x_m + x.a, x_r);
        if (v->l && v->min > v->l->min) v->min = v->l->min;
        if (v->r && v->min > v->r->min) v->min = v->r->min;
        return v;
    }
    T min(node *v, T x, T l, T r) {
        if (!v) return inf;
        if (l == r) return v->x.get(x);
        T m = (l + r) / 2;
        if (m == r) --m;
        if (x <= m) return std::min(v->x.get(x), min(v->l, x, l, m));
        else return std::min(v->x.get(x), min(v->r, x, m + 1, r));
    }
    line min_line(node *v, T x, T l, T r) {
        if (!v) return line(0, inf);
        if (l == r) return v->x;
        T m = (l + r) / 2;
        if (m == r) --m;
        if (x <= m) {
            line res = min_line(v->l, x, l, m);
            return v->x.get(x) <= res.get(x) ? v->x : res;
        } else {
            line res = min_line(v->r, x, m + 1, r);
            return v->x.get(x) <= res.get(x) ? v->x : res;
        }
    }
    T range_min(node *v, T a, T b, T l, T r) {
        if (!v || r < a || b < l) return inf;
        if (a <= l && r <= b) {
            return v->min;
        }
        T m = (l + r) / 2;
        if (m == r) --m;
        T min1 = std::min(v->x.get(std::max(a, l)), v->x.get(std::min(b, r)));
        T min_line = std::min(range_min(v->l, a, b, l, m), range_min(v->r, a, b, m + 1, r));
        return std::min(min1, min_line);
    }
    void enumerate(node *v, T l, T r, std::vector<line> &L, std::vector<std::pair<T, line>> &res) {
        L.push_back(v->x);
        T m = (l + r) / 2;
        if (m == r) --m;
        // [lx, rx]
        auto calc = [&](T lx, T rx) -> void {
            while (true) {
                line a = min_line(lx);
                if (res.empty() || res.back().second != a) res.push_back({lx, a});
                if (a.b == inf) return;
                T intersection = rx + 1; // min(ceil(交点))
                line next_line = line(0, inf);
                for (int i = 0; i < L.size(); i++) {
                    if (L[i].b == inf || L[i].a >= a.a) continue;
                    // (a.a - L[i].a) x = L[i].b - a.b
                    T diff_a = a.a - L[i].a;
                    assert(diff_a != 0);
                    T ceil_x = (L[i].b - a.b + diff_a - 1) / diff_a;
                    if (lx < ceil_x && ceil_x < intersection) {
                        assert(lx < ceil_x);
                        next_line = L[i];
                        intersection = ceil_x;
                    }
                }
                if (intersection > rx) break;
                lx = intersection;
            }
        };
        if (v->l && v->r) {
            enumerate(v->l, l, m, L, res);
            enumerate(v->r, m + 1, r, L, res);
        } else if (!v->l && !v->r) {
            calc(l, r);
        } else if (!v->l) {
            calc(l, m);
            enumerate(v->r, m + 1, r, L, res);
        } else {
            enumerate(v->l, l, m, L, res);
            calc(m + 1, r);
        }
        L.pop_back();
    }
  public:
    li_chao_tree_range_min() : root(nullptr) {}
    
    // 直線ax + bを追加
    void add_line(T a, T b) {
        line x(a, b);
        root = add_line(root, x, xmin, xmax, x.get(xmin), x.get(xmax));
    }

    // 線分ax + b, [l, r)を追加
    void add_segment(T l, T r, T a, T b) {
        assert(l <= r);
        line x(a, b);
        root = add_segment(root, x, l, r - 1, xmin, xmax, x.get(xmin), x.get(xmax));
    }

    // f(x)の最小値
    T min(T x) {
        return min(root, x, xmin, xmax);
    }

    // f(x)の最小値をとる線
    line min_line(T x) {
        return min_line(root, x, xmin, xmax);
    }

    // l <= x < rを満たす整数xでのf(x)の最小値
    T range_min(T l, T r) {
        return range_min(root, l, r - 1, xmin, xmax);
    }

    // {l, f}, fはl, (次のl)において最小値を取る
    std::vector<std::pair<T, line>> enumerate() {
        if (!root) return {{xmin, line(0, inf)}};
        std::vector<std::pair<T, line>> res;
        std::vector<line> L;
        enumerate(root, xmin, xmax, L, res);
        return res;
    }
};
#endif
