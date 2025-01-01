#ifndef _SPLAY_BASE_H_
#define _SPLAY_BASE_H_
#include <cassert>
#include <tuple>
#include <vector>

template<typename T>
struct splay_node {
    using node = splay_node;
    node *lch, *rch, *par;
    T _val;
    splay_node(T x) : lch(nullptr), rch(nullptr), par(nullptr), _val(x) {}
};

template<typename Node, void (*update)(Node*), void (*push)(Node*)> 
struct splay_tree_base {
    static void splay_fast(Node *v){
        push(v);
        while (v->par) {
            Node *p = v->par;
            if (!p->par) {
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

    static Node *leftmost(Node *v) {
        while (v->lch) {
            push(v);
            v = v->lch;
        }
        splay(v);
        return v;
    }
};
#endif