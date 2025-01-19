#ifndef _SEGTREE_SORTABLE2_H_
#define _SEGTREE_SORTABLE2_H_
#include <vector>
#include <algorithm>
#include <cassert>

template<int log, typename S, S (*op)(S, S), S (*e)(), S (*flip)(S)>
struct meldable_binary_trie {
    struct node {
        int sz;
        S sum;
        node *l, *r;
        node() : sz(0), sum(e()), l(nullptr), r(nullptr) {}
        node(S x) : sz(1), sum(x), l(nullptr), r(nullptr) {}
    };
    static int size(node *a) { return a ? a->sz : 0; }
    static S all_prod(node *a) { return a ? a->sum : e(); }
    static node *update(node *a) {
        a->sz = (a->l ? a->l->sz : 0) + (a->r ? a->r->sz : 0);
        a->sum = op((a->l ? a->l->sum : e()), (a->r ? a->r->sum : e()));
        return a;
    }
    static node *meld(node *a, node *b) {
        if (!a || !b) return !a ? b : a;
        a->l = meld(a->l, b->l);
        a->r = meld(a->r, b->r);
        return update(a);
    }
    // [0, r), [r, sz)
    static std::pair<node*, node*> split(node *a, int r) {
        if (r == 0) return {nullptr, a};
        if (r == a->sz) return {a, nullptr};
        int lsz = size(a->l);
        if (lsz <= r) {
            auto [u, v] = split(a->r, r - lsz);
            a->r = u;
            node *tmp = new node();
            tmp->r = v;
            return {update(a), update(tmp)};
        } else {
            auto [u, v] = split(a->l, r);
            a->l = v;
            node *tmp = new node();
            tmp->l = u;
            return {update(tmp), update(a)};
        }
    }
    // prod [0, r)
    static S prod_left(node *a, int r) {
        if (r == 0) return e();
        if (r == a->sz) return a->sum;
        int lsz = size(a->l);
        if (r <= lsz) return prod_left(a->l, r);
        return op(all_prod(a->l), prod_left(a->r, r - lsz));
    }
    // prod [l, sz)
    static S prod_right(node *a, int l) {
        if (l == a->sz) return e();
        if (l == 0) return a->sum;
        int lsz = size(a->l);
        if (l >= lsz) return prod_right(a->r, l - lsz);
        return op(prod_right(a->l, l), all_prod(a->r));
    }
    static node *make_tree(int key, S x) {
        node *res = new node(x);
        for (int i = 0; i < log; i++) {
            node *v = new node();
            if ((key >> i) & 1) {
                v->r = res;
            } else {
                v->l = res;
            }
            res = update(v);
        }
        return res;
    }
};

template<typename S, S (*op)(S, S), S (*e)(), S (*flip)(S)>
struct meldable_treap {
    static unsigned xor128() {
        static unsigned x = 123456789, y = 362436069, z = 521288629, w = 86675123;
        unsigned t = (x ^ (x << 11));
        x = y, y = z, z = w;
        return (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
    }
    struct node {
        unsigned pr;
        int sz, key, maxkey, minkey;
        S val, sum;
        node *l, *r;
        node(int k, S x) : pr(xor128()), sz(1), key(k), maxkey(k), minkey(k), val(x), sum(x), l(nullptr), r(nullptr) {} 
    };
    static int size(node *a) { return a ? a->sz : 0; }
    static S all_prod(node *a) { return a ? a->sum : e(); }
    static node *update(node *a) {
        a->sz = 1;
        a->sum = a->val;
        a->maxkey = a->key;
        a->minkey = a->key;
        if (a->l) {
            a->sz += a->l->sz;
            a->sum = op(a->l->sum, a->sum);
            a->minkey = a->l->minkey;
        }
        if (a->r) {
            a->sz += a->r->sz;
            a->sum = op(a->sum, a->r->sum);
            a->maxkey = a->r->maxkey;
        }
        return a;
    }
    // prod [0, r)
    static S prod_left(node *a, int r) {
        S res = e();
        while (a) {
            int lsz = size(a->l);
            if (r <= lsz) {
                a = a->l;
            } else {
                res = op(res, op(all_prod(a->l), a->val));
                if (lsz + 1 == r) return res;
                r -= lsz + 1;
                a = a->r;
            }
        }
        return res;
    }
    // prod [l, sz)
    static S prod_right(node *a, int l) {
        S res = e();
        while (a) {
            int lsz = size(a->l);
            if (lsz + 1 <= l) {
                l -= lsz + 1;
                a = a->r;
            } else {
                res = op(a->val, op(all_prod(a->r), res));
                if (lsz == l) return res;
                a = a->l;
            }
        }
        return res;
    }
    // [0, r), [r, sz)
    static std::pair<node*, node*> split(node *a, int r) {
        if (r == 0) return {nullptr, a};
        if (r == size(a)) return {a, nullptr};
        int lsz = size(a->l);
        if (r <= lsz) {
            auto [x, y] = split(a->l, r);
            a->l = y;
            return {x, update(a)};
        } else {
            auto [x, y] = split(a->r, r - lsz - 1);
            a->r = x;
            return {update(a), y};
        }
    }
    static std::pair<node*, node*> split_key(node *a, int r) {
        if (!a) return {nullptr, nullptr};
        if (r <= a->minkey) return {nullptr, a};
        if (a->maxkey < r) return {a, nullptr};
        if (r <= a->key) {
            auto [x, y] = split_key(a->l, r);
            a->l = y;
            return {x, update(a)};
        } else {
            auto [x, y] = split_key(a->r, r);
            a->r = x;
            return {update(a), y};
        }
    }
    static node *meld(node *a, node *b) {
        if (!a || !b) return !a ? b : a;
        if (a->pr < b->pr) std::swap(a, b);
        if (b->maxkey <= a->key) {
            a->l = meld(a->l, b);
            return update(a);
        }
        auto [x, y] = split_key(b, a->key);
        a->l = meld(a->l, x);
        a->r = meld(a->r, y);
        return update(a);
    }
    static node *make_tree(int k, S x) {
        return new node(k, x);
    }
};


template<typename S, S (*op)(S, S), S (*e)(), S (*flip)(S)>
struct segtree_sortable {
    using tree_inner = meldable_treap<S, op, e, flip>;
    //using tree_inner = meldable_binary_trie<30, S, op, e, flip>; // キーが[0, 2^30)

    using node_inner = tree_inner::node;
    struct node_outer {
        int sz;
        S sum;
        bool fliped;
        node_inner *v;
        node_outer *l, *r, *p;
        node_outer(bool _flipped, node_inner *_v) : sz(tree_inner::size(_v)), sum(tree_inner::all_prod(_v)), 
            fliped(_flipped), v(_v), l(nullptr), r(nullptr), p(nullptr) {}
    };
    static int size(node_outer *a) { return a ? a->sz : 0; }
    static S all_prod(node_outer *a) { return a ? a->sum : e(); }
    static node_outer *update(node_outer *a) {
        a->sz = tree_inner::size(a->v);
        a->sum = tree_inner::all_prod(a->v);
        if (a->fliped) a->sum = flip(a->sum);
        if (a->l) {
            a->sz += a->l->sz;
            a->sum = op(a->l->sum, a->sum);
        }
        if (a->r) {
            a->sz += a->r->sz;
            a->sum = op(a->sum, a->r->sum);
        }
        return a;
    }

    static void rotate_right(node_outer *a) {
        node_outer *p = a->p;
        if ((p->l = a->r)) a->r->p = p;
        a->r = p, p->p = a;
        update(p), update(a);
    }
    
    static void rotate_left(node_outer *a) {
        node_outer *p = a->p;
        if ((p->r = a->l)) a->l->p = p;
        a->l = p, p->p = a;
        update(p), update(a);
    }

    static node_outer *splay(node_outer *v) {
        while (v->p) {
            node_outer *p = v->p;
            if (!p->p) {
                node_outer *pp = p->p;
                if(p->l == v) {
                    if ((p->l = v->r)) p->l->p = p;
                    v->r = p;
                } else {
                    if ((p->r = v->l)) p->r->p = p;
                    v->l = p;
                }
                update(p);
                p->p = v;
                update(v);
                v->p = pp;
            } else {
                node_outer *pp = p->p, *ppp = pp->p;
                if (pp->l == p) {
                    if (p->l == v) {
                        if ((pp->l = p->r)) pp->l->p = pp;
                        update(pp);
                        p->r = pp, pp->p = p;
                        if ((p->l = v->r)) p->l->p = p;
                        update(p);
                        v->r = p;
                        p->p = v;
                    } else {
                        if ((p->r = v->l)) p->r->p = p;
                        update(p);
                        if ((pp->l = v->r)) pp->l->p = pp;
                        update(pp);
                        v->l = p, v->r = pp;
                        p->p = pp->p = v;
                    }
                    update(v);
                } else {
                    if(p->r == v) {
                        if ((pp->r = p->l)) pp->r->p = pp;
                        update(pp);
                        p->l = pp, pp->p = p;
                        if ((p->r = v->l)) p->r->p = p;
                        update(p);
                        v->l = p;
                        p->p = v;
                    } else {
                        if ((p->l = v->r)) p->l->p = p;
                        update(p);
                        if ((pp->r = v->l)) pp->r->p = pp;
                        update(pp);
                        v->r = p, v->l = pp;
                        p->p = pp->p = v;
                    }
                    update(v);
                }
                if ((v->p = ppp)) {
                    if (ppp->l == pp) ppp->l = v;
                    if (ppp->r == pp) ppp->r = v;
                }
            }
        }
        return v;
    }

    static std::pair<node_outer*, node_outer*> split(node_outer *a, int r) {
        if (r == 0) return {nullptr, a};
        if (r == size(a)) return {a, nullptr};
        while (true) {
            int lsz = size(a->l);
            if (r < lsz) {
                a = a->l;
            } else if (lsz + tree_inner::size(a->v) <= r){
                r -= lsz + tree_inner::size(a->v);
                a = a->r;
            } else {
                r -= lsz;
                break;
            }
        }
        splay(a);
        if (r == 0) {
            node_outer *b = a->l;
            if (b) b->p = nullptr;
            a->l = nullptr;
            return {b, update(a)};
        } else if (r == tree_inner::size(a->v)) {
            node_outer *b = a->r;
            if (b) b->p = nullptr;
            a->r = nullptr;
            return {update(a), b};
        }
        node_outer *d;
        if (!a->fliped) {
            auto [b, c] = tree_inner::split(a->v, r);
            a->v = b;
            d = new node_outer(false, c);
        } else {
            r = tree_inner::size(a->v) - r;
            auto [c, b] = tree_inner::split(a->v, r);
            a->v = b;
            d = new node_outer(true, c);
        }
        d->r = a->r;
        if (d->r) d->r->p = d;
        a->r = nullptr;
        return {update(a), update(d)};
    }

    static std::tuple<node_outer*, node_outer*, node_outer*> split3(node_outer *a, int l, int r) {
        auto [xy, z] = split(a, r);
        auto [x, y] = split(xy, l);
        return {x, y, z};
    }

    static node_outer *merge(node_outer *a, node_outer *b) {
        if (!a || !b) return !a ? b : a;
        while (a->r) a = a->r;
        splay(a);
        a->r = b;
        b->p = a;
        return update(a);
    }

    static node_outer *merge3(node_outer *a, node_outer *b, node_outer *c) {
        return merge(a, merge(b, c));
    }

    static std::pair<node_outer*, S> prod(node_outer *a, int l, int r) {
        if (r == l) return {a, e()};
        auto [x, y, z] = split3(a, l, r);
        S res = y->sum;
        return {merge3(x, y, z), res};
    }

    static node_outer *all_sort(node_outer *a, bool ord) {
        if (!a) return nullptr;
        node_inner *v = nullptr;
        auto dfs = [&](auto &&dfs, node_outer *b) -> void {
            v = tree_inner::meld(v, b->v);
            if (b->l) dfs(dfs, b->l);
            if (b->r) dfs(dfs, b->r);
        };
        dfs(dfs, a);
        *a = node_outer(ord, v);
        return a;
    }

    static node_outer *sort(node_outer *a, int l, int r, bool ord) {
        if (r == l) return a;
        auto [x, y, z] = split3(a, l, r);
        y = all_sort(y, ord);
        y->l = x;
        y->r = z;
        if (x) x->p = y;
        if (z) z->p = y;
        return update(y);
    }

    static node_outer *build(const std::vector<std::pair<int, S>> &V) {
        if (V.empty()) return nullptr;
        auto dfs = [&](auto &&dfs, int l, int r) -> node_outer* {
            if (r - l == 1) {
                auto [key, x] = V[l];
                return new node_outer(false, tree_inner::make_tree(key, x));
            }
            int m = (l + r) / 2;
            auto [key, x] = V[m];
            node_outer *v = new node_outer(false, tree_inner::make_tree(key, x));
            if (l < m) {
                v->l = dfs(dfs, l, m);
                v->l->p = v;
            }
            if (m + 1 < r) {
                v->r = dfs(dfs, m + 1, r);
                v->r->p = v;
            }
            return update(v);
        };
        return dfs(dfs, 0, V.size());
    }
};
#endif