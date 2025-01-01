#ifndef _LI_CHAO_TREE_QUADRATIC_H_
#define _LI_CHAO_TREE_QUADRATIC_H_
#include <algorithm>
#include <vector>

template<typename T, T xmin, T xmax>
struct li_chao_tree_quadratic {
    static constexpr T inf = std::numeric_limits<T>::max();
    struct line {
        T a, b, c;
        line(T a, T b, T c): a(a), b(b), c(c){}
        inline T get(T x){return a * x * x + b * x + c;}
        bool operator != (const line &r){return a != r.a || b != r.b;}
    };
  private:
    struct node {
        line x;
        node *l, *r;
        node(const line &x) : x(x), l(nullptr), r(nullptr){}
    };
    node *root;
    node *add_line(node *v, line &x, T l, T r, T x_l, T x_r) {
        if (!v) return new node(x);
        T t_l = v->x.get(l), t_r = v->x.get(r);
        if (l + 1 == r) {
            if (x_l < t_l) v->x = x;
            return v;
        } else if (t_l <= x_l && t_r <= x_r) {
            return v;
        } else if (t_l >= x_l && t_r >= x_r) {
            v->x = x;
            return v;
        } else {
            T m = (l + r) / 2;
            T t_m = v->x.get(m), x_m = x.get(m);
            if (t_m > x_m) {
                std::swap(v->x, x);
                if (x_l >= t_l) v->l = add_line(v->l, x, l, m, t_l, t_m);
                else v->r = add_line(v->r, x, m, r, t_m, t_r);
            } else {
                if (t_l >= x_l) v->l = add_line(v->l, x, l, m, x_l, x_m);
                else v->r = add_line(v->r, x, m, r, x_m, x_r);
            }
            return v;
        }
    }
    node *add_segment(node *v, line &x, T a, T b, T l, T r, T x_l, T x_r) {
        if (r <= a || b <= l) return v;
        if (a <= l && r <= b) {
            line y(x);
            return add_line(v, y, l, r, x_l, x_r);
        }
        if (v) {
            T t_l = v->x.get(l), t_r = v->x.get(r);
            if (t_l <= x_l && t_r <= x_r) return v;
        } else {
            v = new node(line(0, 0, inf));
        }
        T m = (l + r) / 2;
        T x_m = x.get(m);
        v->l = add_segment(v->l, x, a, b, l, m, x_l, x_m);
        v->r = add_segment(v->r, x, a, b, m, r, x_m, x_r);
        return v;
    }
    T min(node *v, T l, T r, T x) {
        if (!v) return inf;
        if (l + 1 == r) return v->x.get(x);
        T m = (l + r) / 2;
        if (x < m) return std::min(v->x.get(x), min(v->l, l, m, x));
        else return std::min(v->x.get(x), min(v->r, m, r, x));
    }
    line min_line(node *v, T l, T r, T x) {
        if (!v) return line(0, inf);
        if (l + 1 == r) return v->x;
        T m = (l + r) / 2;
        if (x < m) {
            line res = min_line(v->l, l, m, x);
            return v->x.get(x) <= res.get(x) ? v->x : res;
        } else {
            line res = min_line(v->r, m, r, x);
            return v->x.get(x) <= res.get(x) ? v->x : res;
        }
    }
    void enumerate(node *v, T l, T r, std::vector<line> &L, std::vector<std::pair<T, line>> &res) {
        L.push_back(v->x);
        T m = (l + r) / 2;
        auto calc = [&](T lx, T rx) -> void {
            while (true) {
                line a = min_line(lx);
                if (res.empty() || res.back().second != a) res.push_back({lx, a});
                if (a.b == inf) return;
                T intersection = rx; // min(ceil(交点))
                line next_line = line(0, inf);
                for (int i = 0; i < L.size(); i++) {
                    if (L[i].b == inf || L[i].a >= a.a) continue;
                    // (a.a - L[i].a) x = L[i].b - a.b
                    T diff_a = a.a - L[i].a;
                    T ceil_x = (L[i].b - a.b + diff_a - 1) / diff_a;
                    if (lx < ceil_x && ceil_x < intersection) {
                        assert(lx < ceil_x);
                        next_line = L[i];
                        intersection = ceil_x;
                    }
                }
                if (intersection == rx) break;
                lx = intersection;
            }
        };
        if (v->l && v->r) {
            enumerate(v->l, l, m, L, res);
            enumerate(v->r, m, r, L, res);
        } else if (!v->l && !v->r) {
            calc(l, r);
        } else if (!v->l) {
            calc(l, m);
            enumerate(v->r, m, r, L, res);
        } else {
            enumerate(v->l, l, m, L, res);
            calc(m, r);
        }
        L.pop_back();
    }

  public:
    li_chao_tree_quadratic() : root(nullptr) {}

    // 線分ax^2 + bx + cを追加
    void add_line(T a, T b, T c) {
        line x(a, b, c);
        if (a) {
            T mid = -(b / (2 * a));
            if (mid < xmin || xmax < mid) {
                root = add_segment(root, x, xmin, xmax + 1, xmin, xmax + 1, x.get(xmin), x.get(xmax + 1));
            } else {
                root = add_segment(root, x, xmin, mid + 1, xmin, xmax + 1, x.get(xmin), x.get(xmax + 1));
                root = add_segment(root, x, mid, xmax + 1, xmin, xmax + 1, x.get(xmin), x.get(xmax + 1));
            }
        } else {
            root = add_segment(root, x, xmin, xmax + 1, xmin, xmax + 1, x.get(xmin), x.get(xmax + 1));
        }
    }

    // 線分ax^2 + bx + c [l, r)を追加
    void add_segment(T l, T r, T a, T b, T c) {
        line x(a, b, c);
        if (a) {
            T mid = -(b / (2 * a));
            if (mid < l || r <= mid) {
                root = add_segment(root, x, l, r, xmin, xmax + 1, x.get(xmin), x.get(xmax + 1));
            } else {
                root = add_segment(root, x, l, mid + 1, xmin, xmax + 1, x.get(xmin), x.get(xmax + 1));
                root = add_segment(root, x, mid, r, xmin, xmax + 1, x.get(xmin), x.get(xmax + 1));
            }
        } else {
            root = add_segment(root, x, l, r, xmin, xmax + 1, x.get(xmin), x.get(xmax + 1));
        }
    }

    // f(x)の最小値
    T min(T x) {
        return min(root, xmin, xmax + 1, x);
    }

    // f(x)の最小値をとる線
    line min_line(T x) {
        return min_line(root, xmin, xmax + 1, x);
    }

    // {l, f}, fはl, (次のl)において最小値を取る
    std::vector<std::pair<T, line>> enumerate() {
        if (!root) return {{xmin, line(0, inf)}};
        std::vector<std::pair<T, line>> res;
        std::vector<line> L;
        enumerate(root, xmin, xmax + 1, L, res);
        return res;
    }
};
#endif