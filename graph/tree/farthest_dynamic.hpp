#ifndef _FARTHEST_DYNAMIC_H_
#define _FARTHEST_DYNAMIC_H_
#include <algorithm>
#include <queue>

// 頂点にフラグ0/1
// 辺に距離がある(0以上)
// フラグが1の最も遠い頂点
template<typename W>
struct farthest_dynamic {
    static constexpr W minf = std::numeric_limits<W>::min();

    struct T { 
        bool flag;
        W maxdist, maxdist_rev, len, lensum;
        T() : flag(0), maxdist(minf), maxdist_rev(minf), len(0), lensum(0) {}
    };
    
    struct node;
    struct node_in {
        node_in *lch, *rch, *par;
        W wmax;
        node *c, *wmaxnode;
        node_in(node *_self) : lch(nullptr), rch(nullptr), par(nullptr), wmax(_self->x.maxdist), c(_self), wmaxnode(_self) {}
        bool is_root() { return !par; }
    };

    struct node {
        node *lch, *rch, *par;
        bool _flip;
        T x;
        node_in *lic, *lip;
        node() : lch(nullptr), rch(nullptr), par(nullptr), _flip(false), x(), lic(nullptr), lip(nullptr) {}
        bool is_root() { return !par || (par->lch != this && par->rch != this); }
    };

    // フラグfの頂点
    static node *make_node(bool f) { 
        node *v = new node();
        if (f) {
            v->x.flag = 1;
            v->x.maxdist = v->x.maxdist_rev = 0;
        }
        return v;
    }

    // 長さlenの辺
    static node *make_edge(W len) {
        node *v = new node();
        v->x.len = v->x.lensum = len;
        return v;
    }

    static void update(node *v) {
        if (v->lch && v->lch->lip) std::swap(v->lip, v->lch->lip);
        if (v->rch && v->rch->lip) std::swap(v->lip, v->rch->lip);
        T lsum = (v->lch ? v->lch->x : T());
        T rsum = (v->rch ? v->rch->x : T());
        W cmax = (v->lic ? v->lic->wmax : minf);
        if (v->x.flag) {
            v->x.maxdist = lsum.lensum;
            v->x.maxdist_rev = rsum.lensum;
        } else {
            v->x.maxdist = minf;
            v->x.maxdist_rev = minf;
        }
        if (cmax != minf) {
            v->x.maxdist = lsum.lensum + v->x.len + cmax;
            v->x.maxdist_rev = rsum.lensum + v->x.len + cmax;
        }
        if (rsum.maxdist > cmax) v->x.maxdist = lsum.lensum + v->x.len + rsum.maxdist;
        if (lsum.maxdist_rev > cmax) v->x.maxdist_rev = rsum.lensum + v->x.len + lsum.maxdist_rev;
        v->x.maxdist = std::max(v->x.maxdist, lsum.maxdist);
        v->x.maxdist_rev = std::max(v->x.maxdist_rev, rsum.maxdist_rev);
        v->x.lensum = v->x.len + lsum.lensum + rsum.lensum;
    }

    static void update(node_in *v) {
        v->wmax = v->c->x.maxdist;
        v->wmaxnode = v->c;
        if (v->lch && v->wmax < v->lch->wmax) {
            v->wmax = v->lch->wmax;
            v->wmaxnode = v->lch->wmaxnode;
        }
        if (v->rch && v->wmax < v->rch->wmax) {
            v->wmax = v->rch->wmax;
            v->wmaxnode = v->rch->wmaxnode;
        }
    }

    static void push(node_in *v) {}

    static void flip(node *v) {
        v->_flip = !v->_flip;
        std::swap(v->x.maxdist, v->x.maxdist_rev);
        std::swap(v->lch, v->rch);
    }

    static void push(node *v) {
        if (v->_flip) {
            if (v->lch) flip(v->lch);
            if (v->rch) flip(v->rch);
            v->_flip = false;
        }
    }
    
    template<typename Node = node>
    static void splay(Node *v) {
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
        if constexpr (std::is_same<Node, node>::value == true) {
            if (v->lip) {
                v->lip->c = v;
            }
        }
    }
    
    static node* expose(node *v) {
        node *c = nullptr;
        for (node *u = v; u; u = u->par) {
            splay(u);
            node *r = u->rch;
            if (c && r) {
                node_in *p = c->lip;
                std::swap(c->lip, r->lip);
                p->c = r;
                update(p);
                splay<node_in>(p);
                u->lic = p;
            } else if (c) {
                node_in *p = c->lip;
                c->lip = nullptr;
                splay<node_in>(p);
                if (!p->lch || !p->rch) {
                    if (p->rch) std::swap(p->lch, p->rch);
                    if ((u->lic = p->lch)) p->lch->par = nullptr;
                } else {
                    node_in *L = p->lch, *R = p->rch;
                    R->par = nullptr;
                    while (R->lch) R = R->lch;
                    splay<node_in>(R);
                    R->lch = L;
                    L->par = R;
                    update(R);
                    u->lic = R;
                }
            } else if (r) {
                node_in *p = new node_in(r);
                if ((p->rch = u->lic)) {
                    p->rch->par = p;
                    update(p);
                }
                u->lic = r->lip = p;
            }
            u->rch = c;
            update(u);
            c = u;
        }
        splay(v);
        return c;
    }

    // vを根にする
    static void evert(node *v) {
        expose(v);
        flip(v);
    }

    // 非連結であることが保証されている場合
    static void _link(node *p, node *c) {
        evert(c);
        expose(p);
        c->par = p;
        p->rch = c;
        update(p);
    }

    // 辺eを使って繋ぐ(eが孤立点でないと壊れる)
    static void _link_with_edge(node *p, node *e, node *c) {
        evert(c);
        expose(p);
        p->rch = e;
        e->par = p;
        e->lch = nullptr;
        e->rch = c;
        c->par = e;
        update(e);
        update(p);
    }

    // cの親との辺を切る
    static void cut_from_parent(node *c){
        expose(c);
        node *p = c->lch;
        if (p == nullptr) return;
        c->lch = p->par = nullptr;
        update(c);
    }

    // 辺(a, b)があることが保証されている場合
    static void _cut(node *u, node *v) {
        evert(u);
        cut_from_parent(v);
    }

    // 辺eが繋ぐ2頂点を分離(eが次数2でないと壊れる)
    static void _cut_with_edge(node *e) {
        expose(e);
        if (e->lch) {
            // lch & light
            e->lch->par = nullptr;
            node *r = e->lic->c;
            r->lip = nullptr;
            r->par = nullptr;
        } else {
            // light & light
            node *a = e->lic->c;
            node *b = nullptr;
            if (e->lic->lch) {
                b = e->lic->lch->c;
            } else {
                b = e->lic->rch->c;
            }
            a->lip = b->lip = nullptr;
            a->par = b->par = nullptr;
        }
        e->lch = nullptr;
        e->lic = nullptr;
        update(e);
    }

    // 連結なことが保証されている場合
    static node *_lca(node *u, node *v) {
        expose(u);
        return expose(v);
    }
   
    static bool is_there_edge(node *u, node *v) {
        evert(u);
        return u == get_parent(v);
    }

    static node *get_root(node *v) {
        expose(v);
        while (v->lch) {
            push(v);
            v = v->lch;
        }
        splay(v);
        return v;
    }

    static node *get_parent(node *v) {
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

    static W depth(node *v) {
        expose(v);
        return v->x.lensum;
    }

    // 連結であることが保証される場合
    static W _dist(node *u, node *v) {
        return depth(u) + depth(v) - 2 * depth(_lca(u, v));
    }

    // 同じ連結成分か
    static bool is_same(node *u, node *v) {
        if (!u || !v) return false;
        return get_root(u) == get_root(v);
    }

    // フラグ1の頂点がない場合nullptr
    static node *farthest(node *v) {
        evert(v);
        if (v->x.maxdist == minf) return nullptr;
        while (true) {
            push(v);
            W llen = v->lch ? v->lch->x.lensum : 0;
            W a = v->lch ? v->lch->x.maxdist : minf;
            W b = v->lic ? llen + v->x.len + v->lic->wmax : minf;
            W c = v->rch ? llen + v->x.len + v->rch->x.maxdist : minf;
            W mx = std::max({a, b, c});
            if (v->x.flag && mx <= llen) {
                expose(v);
                return v;
            } else if(a == mx) {
                v = v->lch;
            } else if(b == mx) {
                v = v->lic->wmaxnode;
                splay(v);
            } else {
                v = v->rch;
            }
        }
    }
};
#endif