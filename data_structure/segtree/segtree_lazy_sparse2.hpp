#ifndef _SEGTREE_LAZY_SPARSE_2_H_
#define _SEGTREE_LAZY_SPARSE_2_H_
#include <cassert>
#include <algorithm>

template<typename Idx, typename S, S (*op)(S, S), S (*e)(), typename F, S (*mapping)(F, S, Idx), F (*composition)(F, F), F (*id)()>
struct segtree_lazy_sparse {
  private:
    
    static constexpr S pow_monoid(S a, Idx k) {
        S b = e();
        while (k) {
            if (k & 1) b = op(b, a);
            a = op(a, a);
            k >>= 1;
        }
        return b;
    }

    struct node {
        int h;
        Idx lx, rx, Lx, Rx;
        node *l, *r;
        S val, sum, sum_subtree;
        F lz;
        node(Idx _lx, Idx _rx, S _val) : lx(_lx), rx(_rx), Lx(_lx), Rx(_rx), l(nullptr), r(nullptr), 
            val(_val), sum(pow_monoid(_val, rx - lx)), sum_subtree(sum), lz(id()) {}
        int factor() const { return (l ? l->h : 0) - (r ? r->h : 0); }
    };

    static void update(node *v) {
        v->h = 1;
        v->Lx = v->lx;
        v->Rx = v->rx;
        v->sum_subtree = v->sum;
        if (v->l) {
            v->h = v->l->h + 1;
            v->Lx = v->l->Lx;
            v->sum_subtree = op(v->l->sum_subtree, v->sum_subtree);
        }
        if (v->r) {
            v->h = std::max(v->h, v->r->h + 1);
            v->Rx = v->r->Rx;
            v->sum_subtree = op(v->sum_subtree, v->r->sum_subtree);
        }
    } 

    static void push_down(node *v) {
        if (v->lz != id()) {
            if (v->l) all_apply(v->l, v->lz);
            if (v->r) all_apply(v->r, v->lz);
            v->lz = id();
        }
    }

    static void all_apply(node *v, F lz) {
        v->lz = composition(lz, v->lz);
        v->val = mapping(lz, v->val, 1);
        v->sum = mapping(lz, v->sum, v->rx - v->lx);
        v->sum_subtree = mapping(lz, v->sum_subtree, v->Rx - v->Lx);
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
        assert(l < r);
        node *a = nullptr, *c = nullptr;
        if (v->lx < l) a = new node(v->lx, l, v->val);
        if (r < v->rx) c = new node(r, v->rx, v->val);
        v->lx = l, v->rx = r;
        v->sum = pow_monoid(v->val, r - l);
        return {a, c};
    }

    node *root;

  public:
    segtree_lazy_sparse(Idx minf, Idx inf) : root(new node(minf, inf, e())) {}

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
                v->val = v->sum = x;
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

    void apply(Idx l, Idx r, F lz) {
        auto dfs = [&](auto &&dfs, node *v) -> node* {
            if (!v || r <= v->Lx || v->Rx <= l) return v;
            if (l <= v->Lx && v->Rx <= r) {
                all_apply(v, lz);
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
                v->val = mapping(lz, v->val, 1);
                v->sum = mapping(lz, v->sum, v->rx - v->lx);
            }
            update(v);
            return balance(v);
        };
        root = dfs(dfs, root);
    }

    S prod(Idx l, Idx r) {
        auto dfs = [&](auto &&dfs, node *v) -> S {
            if (!v || r <= v->Lx || v->Rx <= l) return e();
            if (l <= v->Lx && v->Rx <= r) return v->sum_subtree;
            push_down(v);
            S mid = e();
            Idx L = std::max(l, v->lx), R = std::min(r, v->rx);
            if (L < R) {
                if (l <= v->lx && v->rx <= r) mid = v->sum;
                else mid = pow_monoid(v->val, R - L);
            }
            return op(dfs(dfs, v->l), op(mid, dfs(dfs, v->r)));
        };
        return dfs(dfs, root);
    }

    template<typename G>
    Idx max_right(Idx l, G g) {
        assert(g(e()));
        S lsum = e();
        auto dfs = [&](auto &&dfs, node *v) -> void {
            if (!v) return;
            if (v->Lx == l && g(op(lsum, v->sum_subtree))) {
                l = v->Rx;
                lsum = op(lsum, v->sum_subtree);
                return;
            }
            push_down(v);
            if (v->rx <= l) {
                dfs(dfs, v->r);
                return;
            }
            if (l < v->lx) dfs(dfs, v->l);
            if (l < v->lx) return;
            S x = (v->lx == l ? v->sum : pow_monoid(v->val, v->rx - l));
            if (g(op(lsum, x))) {
                l = v->rx;
                lsum = op(lsum, x);
            } else {
                Idx len = 1;
                std::vector<S> p2{v->val};
                while (len < v->rx - l) {
                    S s = p2.back();
                    p2.push_back(op(s, s));
                    len *= 2;
                }
                for (int i = (int)p2.size() - 1; i >= 0; i--) {
                    len = Idx(1) << i;
                    if ((v->rx - l) >= len && g(op(lsum, p2[i]))) {
                        l += len;
                        lsum = op(lsum, p2[i]);
                    }
                }
                return;
            }
            dfs(dfs, v->r);
        };
        return dfs(dfs, root);
    }

    template<typename G>
    Idx min_left(Idx r, G g) {
        assert(g(e()));
        S rsum = e();
        auto dfs = [&](auto &&dfs, node *v) -> void {
            if (!v) return;
            if (v->Rx == r && g(op(v->sum_subtree, rsum))) {
                r = v->Lx;
                rsum = op(v->sum_subtree, rsum);
                return;
            }
            push_down(v);
            if (v->lx >= r) {
                dfs(dfs, v->l);
                return;
            }
            if (r > v->rx) dfs(dfs, v->r);
            if (r > v->rx) return;
            S x = (v->rx == r ? v->sum : pow_monoid(v->val, r - v->lx));

            if (g(op(x, rsum))) {
                r = v->lx;
                rsum = op(x, rsum);
            } else {
                Idx len = 1;
                std::vector<S> p2{v->val};
                while (len < r - v->lx) {
                    S s = p2.back();
                    p2.push_back(op(s, s));
                    len *= 2;
                }
                for (int i = (int)p2.size() - 1; i >= 0; i--) {
                    len = Idx(1) << i;
                    if ((r - v->lx) >= len && op(p2[i], rsum)) {
                        r -= len;
                        rsum = op(p2[i], rsum);
                    }
                }
                return;
            }
            dfs(dfs, v->l);
        };
        return dfs(dfs, root);
    }
};
#endif
