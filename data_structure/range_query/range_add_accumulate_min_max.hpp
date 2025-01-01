#ifndef _RANGE_ADD_ACCUMULATE_MIN_MAX_H_
#define _RANGE_ADD_ACCUMULATE_MIN_MAX_H_
#include <algorithm>
#include <numeric>
#include <cassert>

template<typename Idx, typename S>
struct range_add_accumulate_min_max {
    static constexpr S inf = std::numeric_limits<S>::max() / 2;
    static constexpr S minf = std::numeric_limits<S>::min() / 2;
    using self_t = range_add_accumulate_min_max<Idx, S>;
    struct F { S a, b, c; };
    static constexpr F composition(F f, F g) { 
        S a = f.a + g.a;
        S b = std::max(f.b, std::min(g.c, g.b) + f.a);
        S c = std::min(f.c, std::max(f.b, g.c + f.a));
        return {a, b, c};
    }
    static constexpr S mapping(F f, S x) { return std::min(std::max(x + f.a, f.b), f.c); }
    static constexpr F id() { return {0, minf, inf}; }
    
  private:

    struct node {
        int h;
        Idx lx, rx, Lx, Rx;
        node *l, *r;
        bool asc, dsc;
        S val, submin, submax;
        F lz;
        node(Idx _lx, Idx _rx, S _val) : lx(_lx), rx(_rx), Lx(_lx), Rx(_rx), l(nullptr), r(nullptr), 
        asc(true), dsc(true), val(_val), submin(val), submax(val), lz(id()) {}
        int factor() const { return (l ? l->h : 0) - (r ? r->h : 0); }
    };

    static void update(node *v) {
        v->h = 1;
        v->Lx = v->lx;
        v->Rx = v->rx;
        v->submin = v->submax = v->val;
        v->asc = v->dsc = true;
        if (v->l) {
            v->h = v->l->h + 1;
            v->Lx = v->l->Lx;
            if (!v->l->asc || v->l->submax > v->val) v->asc = false;
            if (!v->l->dsc || v->l->submin < v->val) v->dsc = false;
            v->submin = std::min(v->l->submin, v->submin);
            v->submax = std::min(v->l->submax, v->submax);
        }
        if (v->r) {
            v->h = std::max(v->h, v->r->h + 1);
            v->Rx = v->r->Rx;
            if (!v->r->asc || v->submax > v->r->submin) v->asc = false;
            if (!v->r->dsc || v->submin < v->r->submax) v->dsc = false;
            v->submin = std::min(v->r->submin, v->submin);
            v->submax = std::min(v->r->submax, v->submax);
        }
    } 

    static void push_down(node *v) {
        if (v->lz.a != 0 || v->lz.b != minf || v->lz.c != inf) {
            if (v->l) {
                all_apply(v->l, v->lz);
                v->l->asc |= v->asc;
                v->l->dsc |= v->dsc;
            }
            if (v->r) {
                all_apply(v->r, v->lz);
                v->r->asc |= v->asc;
                v->r->dsc |= v->dsc;
            }
            v->lz = id();
        }
    }

    static void all_apply(node *v, F lz) {
        v->lz = composition(lz, v->lz);
        v->val = mapping(lz, v->val);
        v->submin = mapping(lz, v->submin);
        v->submax = mapping(lz, v->submax);
    }

    static node *rotate_right(node *v) {
        node *l = v->l;
        v->l = l->r;
        l->r = v;
        update(v);
        update(l);
        return l;
    }
    
    static node *rotate_left(node *v) {
        node *r = v->r;
        v->r = r->l;
        r->l = v;
        update(v);
        update(r);
        return r;
    }

    static node *balance(node *v) {
        int bf = v->factor();
        if (bf == 2) {
            if (v->l->factor() == -1) {
                v->l = rotate_left(v->l);
                update(v);
            }
            return rotate_right(v);
        } else if(bf == -2) {
            if (v->r->factor() == 1) {
                v->r = rotate_right(v->r);
                update(v);
            }
            return rotate_left(v);
        }
        return v;
    }

    static node *insert_leftmost(node *v, node *u) {
        if (!v) return u;
        push_down(v);
        v->l = insert_leftmost(v->l, u);
        update(v);
        return balance(v);
    }

    static node *insert_rightmost(node *v, node *u) {
        if (!v) return u;
        push_down(v);
        v->r = insert_rightmost(v->r, u);
        update(v);
        return balance(v);
    }

    // vと[l, r)が交差する時に使える
    // [v->lx, l), [r, v-rx)を切り取って返す
    static std::pair<node*, node*> split(node *v, Idx l, Idx r) {
        l = std::max(l, v->lx);
        r = std::min(r, v->rx);
        node *a = nullptr, *c = nullptr;
        if (v->lx < l) a = new node(v->lx, l, v->val);
        if (r < v->rx) c = new node(r, v->rx, v->val);
        v->lx = l, v->rx = r;
        return {a, c};
    }

    node *root;
    int addcnt;
    std::vector<std::tuple<Idx, Idx, S>> history;

  public:
    // 全要素0で初期化
    range_add_accumulate_min_max(Idx N) : root(new node(0, N, 0)), addcnt(0) {}

    void set(Idx p, S x) {
        auto dfs = [&](auto &&dfs, node *v) -> node* {
            if (!v || p < v->Lx || v->Rx <= p) return v;
            push_down(v);
            if (p < v->lx) {
                v->l = dfs(dfs, v->l);
            } else if (v->rx <= p) {
                v->r = dfs(dfs, v->r);
            } else {
                auto [a, c] = split(v, p, p + 1);
                if (a) v->l = insert_rightmost(v->l, a);
                if (c) v->r = insert_leftmost(v->r, c);
                v->val = x;
            }
            update(v);
            return balance(v);
        };
        root = dfs(dfs, root);
    }

    S get(Idx p) {
        node *v = root;
        while (v) {
            push_down(v);
            if (p < v->lx) {
                v = v->l;
            } else if (v->rx <= p) {
                v = v->r;
            } else {
                return v->val;
            }
        }
    }

    // f(x) = min(f(i), 0 <= i <= x)
    void accumulate_min() {
        auto dfs = [&](auto &&dfs, node *v, S lmin) -> node* {
            if (!v) return v;
            if (v->submin >= lmin || v->dsc) {
                if (v->submin >= lmin) v->asc = true;
                v->dsc = true;
                all_apply(v, {0, minf, lmin});
                return v;
            }
            if (v->asc) {
                v->dsc = true;
                all_apply(v, {0, minf, std::min(lmin, v->submin)});
                return v;
            }
            push_down(v);
            if (v->l) {
                v->l = dfs(dfs, v->l, lmin);
                lmin = std::min(lmin, v->l->submin);
            }
            v->val = std::min(v->val, lmin);
            v->r = dfs(dfs, v->r, v->val);
            update(v);
            return balance(v);
        };
        root = dfs(dfs, root, inf);
        history.clear();
    }
    
    // f(x) = max(f(i), 0 <= i <= x)
    void accumulate_max() {
        auto dfs = [&](auto &&dfs, node *v, S lmax) -> node* {
            if (!v) return v;
            if (v->submax <= lmax || v->asc) {
                if (v->submax <= lmax) v->dsc = true;
                v->asc = true;
                all_apply(v, {0, lmax, inf});
                return v;
            }
            if (v->dsc) {
                v->asc = true;
                all_apply(v, {0, std::max(lmax, v->submax), inf});
                return v;
            }
            push_down(v);
            if (v->l) {
                v->l = dfs(dfs, v->l, lmax);
                lmax = std::max(lmax, v->l->submax);
            }
            v->val = std::max(v->val, lmax);
            v->r = dfs(dfs, v->r, v->val);
            update(v);
            return balance(v);
        };
        root = dfs(dfs, root, minf);
        history.clear();
    }

    // f(x) = min(f(i), x <= i < N)
    void accumulate_min_reverse() {
        auto dfs = [&](auto &&dfs, node *v, S rmin) -> node* {
            if (!v) return v;
            if (v->submin >= rmin || v->asc) {
                if (v->submin >= rmin) v->dsc = true;
                v->asc = true;
                all_apply(v, {0, minf, rmin});
                return v;
            }
            if (v->dsc) {
                v->asc = true;
                all_apply(v, {0, minf, std::min(rmin, v->submin)});
                return v;
            }
            push_down(v);
            if (v->r) {
                v->r = dfs(dfs, v->r, rmin);
                rmin = std::min(rmin, v->r->submin);
            }
            v->val = std::min(v->val, rmin);
            v->l = dfs(dfs, v->l, v->val);
            update(v);
            return balance(v);
        };
        root = dfs(dfs, root, inf);
        history.clear();
    }
    
    // f(x) = max(f(i), x <= i < N)
    void accumulate_max_reverse() {
        auto dfs = [&](auto &&dfs, node *v, S rmax) -> node* {
            if (!v) return v;
            if (v->submax <= rmax || v->dsc) {
                if (v->submax <= rmax) v->asc = true;
                v->dsc = true;
                all_apply(v, {0, rmax, inf});
                return v;
            }
            if (v->asc) {
                v->dsc = true;
                all_apply(v, {0, std::max(rmax, v->submax), inf});
                return v;
            }
            push_down(v);
            if (v->r) {
                v->r = dfs(dfs, v->r, rmax);
                rmax = std::max(rmax, v->r->submax);
            }
            v->val = std::max(v->val, rmax);
            v->l = dfs(dfs, v->l, v->val);
            update(v);
            return balance(v);
        };
        root = dfs(dfs, root, minf);
        history.clear();
    }

    // (fのl <= x < r) <- min(max(f(x) + add, lower), upper)
    void apply(Idx l, Idx r, S add, S lower, S upper) {
        auto dfs = [&](auto &&dfs, node *v) -> node* {
            if (!v || r <= v->Lx || v->Rx <= l) return v;
            if (l <= v->Lx && v->Rx <= r) {
                all_apply(v, {add, lower, upper});
                return v;
            }
            push_down(v);
            v->l = dfs(dfs, v->l);
            v->r = dfs(dfs, v->r);
            Idx L = std::max(l, v->lx), R = std::min(r, v->rx);
            if (L < R) {
                if (L != v->lx || R != v->rx) {
                    auto [a, c] = split(v, l, r);
                    if (a) v->l = insert_rightmost(v->l, a);
                    if (c) v->r = insert_leftmost(v->r, c);
                }
                v->val = mapping({add, lower, upper}, v->val);
            }
            update(v);
            return balance(v);
        };
        root = dfs(dfs, root);
    }

    void add(Idx l, Idx r, S x) { 
        if (x) {
            history.push_back({l, r, x});
            addcnt++;
        }
        apply(l, r, x, minf, inf);
    }
    void chmin(Idx l, Idx r, S x) { apply(l, r, 0, minf, x); }
    void chmax(Idx l, Idx r, S x) { apply(l, r, 0, x, inf); }

    // min(f[l, r))
    S get_min(Idx l, Idx r) {
        auto dfs = [&](auto &&dfs, node *v) -> S {
            if (!v || r <= v->Lx || v->Rx <= l) return inf;
            if (l <= v->Lx && v->Rx <= r) return v->submin;
            push_down(v);
            S mid = inf;
            Idx L = std::max(l, v->lx), R = std::min(r, v->rx);
            if (L < R) mid = v->val;
            return std::min(dfs(dfs, v->l), std::min(mid, dfs(dfs, v->r)));
        };
        return dfs(dfs, root);
    }

    // max(f[l, r))
    S get_max(Idx l, Idx r) {
        auto dfs = [&](auto &&dfs, node *v) -> S {
            if (!v || r <= v->Lx || v->Rx <= l) return minf;
            if (l <= v->Lx && v->Rx <= r) return v->submax;
            push_down(v);
            S mid = minf;
            Idx L = std::max(l, v->lx), R = std::min(r, v->rx);
            if (L < R) mid = v->val;
            return std::max(dfs(dfs, v->l), std::max(mid, dfs(dfs, v->r)));
        };
        return dfs(dfs, root);
    }
    
    // K回の加算と0回以上の累積min/maxを好きな順序で行った後の状態はK回以下の加算で表現できる
    // これを復元
    // chmin/chmaxを混ぜると壊れる
    std::vector<std::tuple<Idx, Idx, S>> restore() {
        auto res = history;
        std::vector<std::pair<Idx, S>> diff;
        for (auto [l, r, x] : history) {
            diff.push_back({l, -x});
            diff.push_back({r, x});
        }
        std::sort(diff.begin(), diff.end());
        int i = 0;
        S dx = 0;
        auto dfs = [&](auto &&dfs, node *v) -> void {
            if (!v) return;
            push_down(v);
            dfs(dfs, v->l);
            while (i < diff.size() && diff[i].first <= v->lx) dx += diff[i++].second;
            S x = v->val + dx;
            if (!res.empty()) {
                auto [_l, _r, _x] = res.back();
                if (_r == v->lx && _x == x) {
                    res.back() = {_l, v->rx, _x};
                } else {
                    res.push_back({v->lx, v->rx, x});
                }
            } else {
                res.push_back({v->lx, v->rx, x});
            }
            dfs(dfs, v->r);
        };
        dfs(dfs, root);
        return res;
    }

    void clear() {
        Idx N = root-> Rx;
        root = new node(0, N, 0);
        addcnt = 0;
        history.clear();
    }

    void swap(self_t &g) {
        std::swap(root, g.root);
        std::swap(addcnt, g.addcnt);
        std::swap(history, g.history);
    }

    // f + g (gは空になる)
    void meld(self_t &g) {
        if (addcnt < g.addcnt) swap(g);
        for (auto [l, r, x] : g.restore()) {
            add(l, r, x);
        }
        g.clear();
    }
};
#endif