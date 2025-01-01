#ifndef _TOPTREE_H_
#define _TOPTREE_H_
#include "../../data_structure/tree_base/splay_tree.hpp"

template<typename H, typename L, H (*eH)(), L (*toL)(const H&), L (*mergeLL)(const L&, const L&), H (*mergeHL)(const H&, const L&), H (*mergeHH)(const H&, const H&), bool use_rev>
struct toptree {
  private:
    struct S {
        L x, xsum;
        S(L _x) : x(_x), xsum(x) {}
    };
    using VL = splay_node<S>;
    static constexpr void updateL(VL *v) {
        v->_val.xsum = v->_val.x;
        if (v->lch) {
            v->_val.xsum = mergeLL(v->lch->_val.xsum, v->_val.xsum);
        }
        if (v->rch) {
            v->_val.xsum = mergeLL(v->_val.xsum, v->rch->_val.xsum);
        }
    }
    static constexpr void pushL(VL *v) {}
    using TL = splay_tree_base<VL, updateL, pushL>;
    
    struct VH {
        H y, ysum, ysumrev;
        VL *lic, *lip;
        VH *lch, *rch, *par;
        bool _flip;
        VH(H _y) : y(_y), ysum(_y), ysumrev(_y), lic(nullptr), lip(nullptr), lch(nullptr), rch(nullptr), par(nullptr), _flip(false) {}
        bool is_root() { return !par || (par->lch != this && par->rch != this); }
    };

    static constexpr void update(VH *v) {
        if constexpr (!use_rev) {
            v->ysum = v->y;
            H c = eH();
            if (v->lch) {
                v->ysum = mergeHH(v->lch->ysum, v->ysum);
                if (v->lch->lip) std::swap(v->lip, v->lch->lip);
            }
            if (v->rch) {
                c = v->rch->ysum;
                if (v->rch->lip) std::swap(v->lip, v->rch->lip);
            }
            if (v->lic) {
                c = mergeHL(c, v->lic->_val.xsum);
            }
            v->ysum = mergeHH(v->ysum, c);
        } else {
            v->ysum = v->ysumrev = v->y;
            H c = eH(), crev = eH();
            if (v->lch) {
                v->ysum = mergeHH(v->lch->ysum, v->ysum);
                crev = v->lch->ysumrev;
                if (v->lch->lip) std::swap(v->lip, v->lch->lip);
            }
            if (v->rch) {
                v->ysumrev = mergeHH(v->rch->ysumrev, v->ysumrev);
                c = v->rch->ysum;
                if (v->rch->lip) std::swap(v->lip, v->rch->lip);
            }
            if (v->lic) {
                c = mergeHL(c, v->lic->_val.xsum);
                crev = mergeHL(crev, v->lic->_val.xsum);
            }
            v->ysum = mergeHH(v->ysum, c);
            v->ysumrev = mergeHH(v->ysumrev, crev);
        }
    }

    static constexpr void push(VH *v) {
        if (v->_flip) {
            if (v->lch) flip(v->lch);
            if (v->rch) flip(v->rch);
            v->_flip = false;
        }
    }

    static constexpr void flip(VH *v) {
        v->_flip = !v->_flip;
        if constexpr (use_rev) std::swap(v->ysum, v->ysumrev);
        std::swap(v->lch, v->rch);
    }

    static void splay(VH *v) {
        push(v);
        while (!v->is_root()) {
            VH *p = v->par;
            if (p->is_root()) {
                push(p), push(v);
                VH *pp = p->par;
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
                VH *pp = p->par;
                VH *ppp = pp->par;
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

  public:
    using node = VH;

    static node *make_node(H x) { return new node(x); }

    static node *expose(node *v) {
        node *c = nullptr;
        for (node *u = v; u; u = u->par) {
            splay(u);
            node *r = u->rch;
            bool f = c, g = r;
            if (f && g) {
                std::swap(c->lip, r->lip);
                VL *lip = r->lip;
                TL::splay(lip);
                lip->_val.x = toL(r->ysum);
                updateL(lip);
                u->lic = lip;
            } else if (f) {
                VL *lip = c->lip;
                c->lip = nullptr;
                TL::splay(lip);
                if (!lip->rch) {
                    if (lip->lch) lip->lch->par = nullptr;
                    u->lic = lip->lch;
                } else {
                    VL *a = lip->lch, *b = lip->rch;
                    b->par = nullptr;
                    b = TL::leftmost(b);
                    b->lch = a;
                    if (a) a->par = b;
                    updateL(b);
                    u->lic = b;
                }
            } else if (g) {
                VL *lip = new VL(S(toL(r->ysum)));
                r->lip = lip;
                if ((lip->lch = u->lic)) {
                    u->lic->par = lip;
                    updateL(lip);
                }
                u->lic = lip;
            }
            u->rch = c;
            update(u);
            c = u;
        }
        splay(v);
        return c;
    }

    static node *evert(node *v) {
        expose(v);
        flip(v);
        push(v);
        return v;
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

    // 同じ連結成分か
    static bool is_same(node *u, node *v) {
        if (!u || !v) return false;
        return get_root(u) == get_root(v);
    }

    // 連結なことが保証されている場合
    static node *_lca(node *u, node *v) {
        expose(u);
        return expose(v);
    }

    // 非連結であることが保証されている場合
    static void _link(node *p, node *c) {
        evert(c);
        expose(p);
        c->par = p;
        p->rch = c;
        update(p);
    }

    // 辺(a, b)があることが保証されている場合
    static bool _cut(node *u, node *v) {
        evert(u);
        return cut_from_parent(v);
    }

    // cの親との辺を切る, false: 辺を切れなかった
    static bool cut_from_parent(node *c) {
        expose(c);
        node *p = c->lch;
        if (!p) return false;
        c->lch = p->par = nullptr;
        update(c);
        return true;
    }

    static void set(node *v, H x) {
        expose(v);
        v->y = x;
        update(v);
    }

    static H get(node *v) {
        expose(v);
        return v->y;
    }

    // vの部分木
    static L subtree_prod(node *v) {
        expose(v);
        if (!v->lch) {
            return toL(v->ysum);
        } else {
            H r = eH();
            if (v->lic) r = mergeHL(r, v->lic->_val.xsum);
            return toL(mergeHH(v->y, r));
        }
    }

    // vを根にしたと仮定して全体クエリ(実際にはevertしない)
    static L all_prod(node *v) {
        expose(v);
        if (!v->lch) {
            return toL(v->ysum);
        } else {
            L parrev;
            if constexpr (!use_rev) {
                parrev = toL(v->lch->ysum);
            } else {
                parrev = toL(v->lch->ysumrev);
            }
            if (v->lic) parrev = mergeLL(parrev, v->lic->_val.xsum);
            return toL(mergeHL(v->y, parrev));
        }
    }
};
#endif