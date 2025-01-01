#ifndef _LCT_SUBTREE_SUM_H_
#define _LCT_SUBTREE_SUM_H_
#include <cassert>
#include <algorithm>

template<typename S>
struct lct_subtree_sum {
    struct node {
        node *l, *r, *p;
        int sz_all, sz_light;
        bool flip;
        S val, sum_light, sum_all;
        S lz, lz_hist;
        node(S _val = 0) : l(nullptr), r(nullptr), p(nullptr), sz_all(1), sz_light(0), flip(false), val(_val), sum_light(0), sum_all(_val), lz(0), lz_hist(lz) {}
        bool is_root() { return !p || (p->l != this && p->r != this); }
    };
    static node *make_node(S val) { return new node(val); }
    lct_subtree_sum() {}

  private:
    static void update(node *v) {
        v->sz_all = 1 + v->sz_light;
        v->sum_all = v->val + v->sum_light;
        if (v->l) {
            v->sz_all += v->l->sz_all;
            v->sum_all = v->l->sum_all + v->sum_all;
        }
        if (v->r) {
            v->sz_all += v->r->sz_all;
            v->sum_all = v->sum_all + v->r->sum_all;
        }
    }

    static void propagate(node *v, S x) {
        v->lz = v->lz + x;
        v->val = v->val + x;
        v->sum_light = v->sum_light + x * v->sz_light;
        v->sum_all = v->sum_all + x * v->sz_all;
    }

    static void flip(node *v) {
        std::swap(v->l, v->r);
        v->flip ^= 1;
    }

    static void push_down(node *v) {
        if (v->flip){
            if (v->l) flip(v->l);
            if (v->r) flip(v->r);
            v->flip = false;
        }
        if (v->l) fetch(v->l);
        if (v->r) fetch(v->r);
    }
    
    static void modify(node *v) {
        if (!v || !v->p) return;
        v->lz_hist = v->p->lz;
    }
    
    static void fetch(node *v) {
        node *p = v->p;
        if (!p || p->lz == v->lz_hist) return;
        S diff = p->lz - v->lz_hist;
        propagate(v, diff);
        v->lz_hist = p->lz;
    }

    static void rotate_right(node *v) {
        node *p = v->p, *pp = p->p;
        if ((p->l = v->r)) v->r->p = p, modify(v->r);
        v->r = p, p->p = v;
        modify(p);
        update(p), update(v);
        if ((v->p = pp)) {
            if (pp->l == p) pp->l = v;
            if (pp->r == p) pp->r = v;
            modify(v);
            update(pp);
        }
    }

    static void rotate_left(node *v) {
        node *p = v->p, *pp = p->p;
        if ((p->r = v->l)) v->l->p = p, modify(v->l);
        v->l = p, p->p = v;
        modify(p);
        update(p), update(v);
        if ((v->p = pp)){
            if (pp->l == p) pp->l = v;
            if (pp->r == p) pp->r = v;
            modify(v);
            update(pp);
        }
    }

    static void splay(node *v) {
        push_down(v);
        while (!v->is_root()) {
            node *p = v->p;
            if (p->is_root()){
                fetch(p);
                push_down(p), push_down(v);
                if (p->l == v) rotate_right(v);
                else rotate_left(v);
            } else {
                node *pp = p->p;
                fetch(pp);
                push_down(pp), push_down(p), push_down(v);
                if (pp->l == p) {
                    if (p->l == v) rotate_right(p);
                    else rotate_left(v);
                    rotate_right(v);
                } else {
                    if (p->r == v) rotate_left(p);
                    else rotate_right(v);
                    rotate_left(v);
                }
            }
        }
    }
    
  public:
    static node *get_root(node *v) {
        expose(v);
        while (v->l) {
            push_down(v);
            v = v->l;
        }
        splay(v);
        return v;
    }

    static node *get_parent(node *v) {
        expose(v);
        if (!v->l) return nullptr;// aが根
        push_down(v);
        v = v->l;
        while (v->r) {
            push_down(v);
            v = v->r;
        }
        splay(v);
        return v;
    }
    
    // 非連結であることが保証されている場合
    static void _link(node *p, node *c) {
        evert(c);
        expose(p);
        c->p = p;
        p->r = c;
        modify(c);
        update(p);
    }

    // cの親との辺を切る, false: 辺を切れなかった
    static void cut_from_parent(node *c) {
        expose(c);
        node *p = c->l;
        if (p == nullptr) return;
        c->l = p->p = nullptr;
        update(c);
    }

    // 辺(a, b)があることが保証されている場合
    static void _cut(node *u, node *v) {
        evert(u);
        cut_from_parent(v);
    }

    // vを根にする
    static node *evert(node *v) {
        expose(v);
        flip(v);
        push_down(v);
        return v;
    }

    static node *expose(node *v) {
        node *c = nullptr;
        for (node *u = v; u; u = u->p) {
            splay(u);
            if (u->r){
                fetch(u->r);
                u->sz_light += u->r->sz_all;
                u->sum_light = u->sum_light + u->r->sum_all;
            }
            if (c) {
                fetch(c);
                u->sz_light -= c->sz_all;
                u->sum_light = u->sum_light - c->sum_all;
            }
            u->r = c;
            update(u);
            c = u;
        }
        splay(v);
        return c;
    }

    // val[v] = x
    static void set(node *v, S x) {
        expose(v);
        v->val = x;
        update(v);
    }

    // val[v]
    static S get(node *v) {
        expose(v);
        return v->val;
    }

    // 呼ぶ前に根をevertしておく
    static void apply_subtree(node *v, S x) {
        node *p = get_parent(v);
        if (p){
            evert(p);
            cut_from_parent(v);
            propagate(v, x);
            _link(p, v);
        } else {
            propagate(v, x);
        }
    }

    // 呼ぶ前に根をevertしておく
    static S prod_subtree(node *v) {
        expose(v);
        return v->val + v->sum_light;
    }

    static bool is_same(node *u, node *v) {
        if (!u || !v) return false;
        return get_root(u) == get_root(v);
    }

    // 部分木のサイズ
    static int size_subtree(node *v) {
        expose(v);
        return 1 + v->sz_light;
    }

    // 連結なことが保証されている場合
    static node *_lca(node *u, node *v) {
        expose(u);
        return expose(v);
    }

    // 辺u-vがあるか
    static bool is_there_edge(node *u, node *v) {
        evert(u);
        return u == get_parent(v);
    }
};
#endif