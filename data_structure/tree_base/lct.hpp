#ifndef _LCT_BASE_H_
#define _LCT_BASE_H_
#include <algorithm>

template<typename T>
struct lct_node {
    using _T = T;
    lct_node *lch, *rch, *par;
    bool _flip;
    T _val;
    lct_node(T x) : lch(nullptr), rch(nullptr), par(nullptr), _flip(false), _val(x) {}
    bool is_root() { return !par || (par->lch != this && par->rch != this); }
};

// _push, _flipは値のみ変化させる関数
template<typename Node, void (*update)(Node*), void (*_push)(Node*), void (*_flip)(Node *v)>
struct lct_base {

    static Node *make_node(typename Node::_T x) {
        return new Node(x);
    }
    static void flip(Node *v) {
        v->_flip = !v->_flip;
        _flip(v);
        std::swap(v->lch, v->rch);
    }

    static void push(Node *v) {
        if (v->_flip) {
            if (v->lch) flip(v->lch);
            if (v->rch) flip(v->rch);
            v->_flip = false;
        }
        _push(v);
    }
    
    static void rotate_right(Node *v) {
        Node *p = v->par, *pp = p->par;
        if ((p->lch = v->rch)) v->rch->par = p;
        v->rch = p, p->par = v;
        update(p), update(v);
        if ((v->par = pp)) {
            if (pp->lch == p) pp->lch = v;
            if (pp->rch == p) pp->rch = v;
            update(pp);
        }
    }

    static void rotate_left(Node *v) {
        Node *p = v->par, *pp = p->par;
        if ((p->rch = v->lch)) v->lch->par = p;
        v->lch = p, p->par = v;
        update(p), update(v);
        if ((v->par = pp)) {
            if (pp->lch == p) pp->lch = v;
            if (pp->rch == p) pp->rch = v;
            update(pp);
        }
    }
  
    static void splay_ordinary(Node *v) {
        push(v);
        while (!v->is_root()) {
            Node *p = v->par;
            if (p->is_root()) { 
                push(p), push(v);
                if (p->lch == v) rotate_right(v);
                else rotate_left(v);
            } else {
                Node *pp = p->par;
                push(pp), push(p), push(v);
                if (pp->lch == p){
                    if (p->lch == v) rotate_right(p);
                    else rotate_left(v);
                    rotate_right(v);
                } else {
                    if (p->rch == v) rotate_left(p);
                    else rotate_right(v);
                    rotate_left(v);
                }
            }
        }
    }

    static void splay_fast(Node *v){
        push(v);
        while (!v->is_root()) {
            Node *p = v->par;
            if (p->is_root()) {
                push(p), push(v);
                Node *pp = p->par;
                if(p->lch == v) {
                    if ((p->lch = v->rch)) p->lch->par = p;
                    update(p);
                    v->rch = p;
                    p->par = v;
                    update(v);
                } else {
                    if ((p->rch = v->lch)) p->rch->par = p;
                    update(p);
                    v->lch = p;
                    p->par = v;
                    update(v);
                }
                v->par = pp;
            } else {
                Node *pp = p->par;
                Node *ppp = pp->par;
                push(pp), push(p), push(v);
                if (pp->lch == p) {
                    if (p->lch == v) {
                        if ((pp->lch = p->rch)) pp->lch->par = pp;
                        update(pp);
                        p->rch = pp;
                        pp->par = p;
                        if ((p->lch = v->rch)) p->lch->par = p;
                        update(p);
                        v->rch = p;
                        p->par = v;
                        update(v);
                    } else {
                        if ((p->rch = v->lch)) p->rch->par = p;
                        update(p);
                        if ((pp->lch = v->rch)) pp->lch->par = pp;
                        update(pp);
                        v->lch = p;
                        v->rch = pp;
                        p->par = pp->par = v;
                        update(v);
                    }
                } else {
                    if(p->rch == v) {
                        if ((pp->rch = p->lch)) pp->rch->par = pp;
                        update(pp);
                        p->lch = pp;
                        pp->par = p;
                        if ((p->rch = v->lch)) p->rch->par = p;
                        update(p);
                        v->lch = p;
                        p->par = v;
                        update(v);
                    } else {
                        if ((p->lch = v->rch)) p->lch->par = p;
                        update(p);
                        if ((pp->rch = v->lch)) pp->rch->par = pp;
                        update(pp);
                        v->rch = p;
                        v->lch = pp;
                        p->par = pp->par = v;
                        update(v);
                    }
                }
                if ((v->par = ppp)) {
                    if (ppp->lch == pp) ppp->lch = v;
                    if (ppp->rch == pp) ppp->rch = v;
                }
            }
        }
    }

    static constexpr auto splay = splay_fast;

    static Node* expose(Node *v) {
        Node *c = nullptr;
        for (Node *u = v; u; u = u->par) {
            splay(u);
            u->rch = c;
            update(u);
            c = u;
        }
        splay(v);
        return c;
    }

    // vを根にする
    static void evert(Node *v) {
        expose(v);
        flip(v);
    }

    // 非連結であることが保証されている場合
    static void _link(Node *p, Node *c) {
        evert(c);
        expose(p);
        c->par = p;
        p->rch = c;
        update(p);
    }

    // cの親との辺を切る
    static void cut_from_parent(Node *c){
        expose(c);
        Node *p = c->lch;
        if (p == nullptr) return;
        c->lch = p->par = nullptr;
        update(c);
    }

    // 辺(a, b)があることが保証されている場合
    static void _cut(Node *u, Node *v) {
        evert(u);
        cut_from_parent(v);
    }

    // 連結なことが保証されている場合
    static Node *_lca(Node *u, Node *v) {
        expose(u);
        return expose(v);
    }

    template<int (*get_sz)(Node*)>
    static int depth(Node *v) {
        expose(v);
        return get_sz(v) - 1;
    }

    // 連結であることが保証されている場合
    template<int (*get_sz)(Node*)>
    static int _dist(Node *u, Node *v) {
        return depth<get_sz>(u) + depth<get_sz>(v) - 2 * depth<get_sz>(_lca(u, v));
    }
   
    static bool is_there_edge(Node *u, Node *v) {
        evert(u);
        return u == get_parent(v);
    }

    static Node *get_root(Node *v) {
        expose(v);
        while (v->lch) {
            push(v);
            v = v->lch;
        }
        splay(v);
        return v;
    }

    static Node *get_parent(Node *v) {
        expose(v);
        if (!v->lch) return nullptr;// aが根
        push(v);
        v = v->lch;
        while (v->rch) {
            push(v);
            v = v->rch;
        }
        splay(v);
        return v;
    }

    // 同じ連結成分か
    static bool is_same(Node *u, Node *v) {
        if (!u || !v) return false;
        return get_root(u) == get_root(v);
    }

    // depth(v) < kの場合根を返す
    template<int (*get_sz)(Node*)>
    static Node *la(Node *v, int k) {
        expose(v);
        k = std::max(0, get_sz(v) - 1 - k);
        while (true) {
            push(v);
            int lsz = v->lch ? get_sz(v->lch) : 0;
            if (lsz == k) break;
            if (lsz > k) v = v->lch;
            else v = v->rch, k -= lsz + 1;
        }
        splay(v);
        return v;
    }

    // s-tパスにのk番目
    // 0 <= k <= dist(s, t)でない場合 nullptr
    // 連結であることが保証される場合
    template<int (*get_sz)(Node*)>
    static Node *_jump_on_tree(Node *s, Node *t, int k) {
        if (k < 0) return nullptr;
        if (k == 0) return s;
        Node *l = _lca(s, t);
        int dl = depth<get_sz>(l), ds = depth<get_sz>(s), dt = depth<get_sz>(t);
        int d = ds + dt - 2 * dl;
        if (k <= ds - dl) return la<get_sz>(s, k);
        else if (k <= d) return la<get_sz>(t, d - k);
        else return nullptr;
    }
};
#endif