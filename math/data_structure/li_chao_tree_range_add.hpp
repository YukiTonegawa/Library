#ifndef _LI_CHAO_TREE_RANGE_ADD_H_
#define _LI_CHAO_TREE_RANGE_ADD_H_
#include <algorithm>
#include <vector>

template<typename T, T xmin, T xmax>
struct li_chao_tree_range_add {
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
        T lazy;
        node(const line &x) : x(x), l(nullptr), r(nullptr), lazy(0){}
    };
    node *root;
  
    void push_lazy(node *v) {
        if (v->lazy == 0) return;
        if (v->l) propagate(v->l, v->lazy);
        if (v->r) propagate(v->r, v->lazy);
        v->lazy = 0;
    }
    void propagate(node *v, T lazy) {
        v->lazy += lazy;
        if (v->x.b != inf) v->x.b += lazy;
    }
    // vの直線をv->l, v->rにpushする
    void push_line(node *v, T l, T r) {
        T m = (l + r) / 2;
        if (m == r) --m;
        T xl = v->x.get(l), xm = v->x.get(m), xr = v->x.get(r);
        v->l = add_line(v->l, v->x, l, m, xl, xm);
        v->r = add_line(v->r, v->x, m + 1, r, xm + v->x.a, xr);
        v->x = {0, inf};
    }
    node *add_line(node *v, line &x, T l, T r, T x_l, T x_r) {
        if (!v) return new node(x);
        push_lazy(v);
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
            if (m == r) --m;
            T t_m = v->x.get(m), x_m = x.get(m);
            if (t_m > x_m) {
                std::swap(v->x, x);
                if (x_l >= t_l) v->l = add_line(v->l, x, l, m, t_l, t_m);
                else v->r = add_line(v->r, x, m + 1, r, t_m + x.a, t_r);
            } else {
                if (t_l >= x_l) v->l = add_line(v->l, x, l, m, x_l, x_m);
                else v->r = add_line(v->r, x, m + 1, r, x_m + x.a, x_r);
            }
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
            push_lazy(v);
        } else {
            v = new node(line(0, inf));
        }
        T m = (l + r) / 2;
        if (m == r) --m;
        T x_m = x.get(m);
        v->l = add_segment(v->l, x, a, b, l, m, x_l, x_m);
        v->r = add_segment(v->r, x, a, b, m + 1, r, x_m + x.a, x_r);
        return v;
    }
    void add(node *v, T x, T a, T b, T l, T r) {
        if (!v || r < a || b < l) return;
        if (a <= l && r <= b) {
            propagate(v, x);
            return;
        }
        push_lazy(v);
        push_line(v, l, r);
        T m = (l + r) / 2;
        if (m == r) --m;
        add(v->l, x, a, b, l, m);
        add(v->r, x, a, b, m + 1, r);
    }
    T min(node *v, T x, T l, T r) {
        if (!v) return inf;
        if (l == r) return v->x.get(x);
        push_lazy(v);
        T m = (l + r) / 2;
        if (m == r) --m;
        if (x <= m) return std::min(v->x.get(x), min(v->l, x, l, m));
        else return std::min(v->x.get(x), min(v->r, x, m + 1, r));
    }
    line min_line(node *v, T x, T l, T r) {
        if (!v) return line(0, inf);
        if (l == r) return v->x;
        push_lazy(v);
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
  public:
    li_chao_tree_range_add() : root(nullptr) {}
    
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

    // [l, r)にxを足す
    void add(T l, T r, T x) {
        add(root, x, l, r - 1, xmin, xmax);
    }

    // f(x)の最小値
    T min(T x) {
        return min(root, x, xmin, xmax);
    }

    // f(x)の最小値をとる線
    line min_line(T x) {
        return min_line(root, x, xmin, xmax);
    }
};
#endif