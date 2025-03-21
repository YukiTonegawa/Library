#ifndef _LCT_MIN_EDGE_H_
#define _LCT_MIN_EDGE_H_
#include "../../data_structure/tree_base/lct.hpp"

template<typename S, bool (*cmp)(S, S), S (*e)()>
struct lct_min_node {
  public:
    struct T {
        int a, b;
        int sz;
        S x, xsum;
        T(S _x) : sz(1), x(_x), xsum(_x) {}
    };
    
    struct node {
        using _T = T;
        node *lch, *rch, *par, *min_node;
        bool _flip;
        T _val;
        node(T x) : lch(nullptr), rch(nullptr), par(nullptr), min_node(this), _flip(false), _val(x) {}
        bool is_root() { return !par || (par->lch != this && par->rch != this); }
    };
  
  private:
    static constexpr void update(node *v) {
        v->_val.sz = 1;
        v->_val.xsum = v->_val.x;
        v->min_node = v;
        if (v->lch) {
            v->_val.sz += v->lch->_val.sz;
            if (cmp(v->lch->_val.xsum, v->_val.xsum)) {
                v->_val.xsum = v->lch->_val.xsum;
                v->min_node = v->lch->min_node;
            }
        }
        if (v->rch) {
            v->_val.sz += v->rch->_val.sz;
            if (cmp(v->rch->_val.xsum, v->_val.xsum)) {
                v->_val.xsum = v->rch->_val.xsum;
                v->min_node = v->rch->min_node;
            }
        }
    }
    static constexpr void push(node *v) {}
    static constexpr void flip(node *v) {}
    static constexpr int get_sz(node *v) { return v->_val.sz; }
    using LCT = lct_base<node, update, push, flip>;

  public:
    static node *make_node(S x) { return LCT::make_node(T(x)); }
    static constexpr auto expose = LCT::expose;
    static constexpr auto evert = LCT::evert;
    static constexpr auto _link = LCT::_link;
    static constexpr auto _cut = LCT::_cut;
    static constexpr auto cut_from_parent = LCT::cut_from_parent;
    static constexpr auto get_root = LCT::get_root;
    static constexpr auto get_parent = LCT::get_parent;
    static constexpr auto is_same = LCT::is_same;
    static constexpr auto _lca = LCT::_lca;
    static node *la(node *v, int k) { return LCT::template la<get_sz>(v, k); }
    static node *_jump_on_tree(node *s, node *t, int k) { return LCT::template _jump_on_tree<get_sz>(s, t, k); }
    static int depth(node *v) { return LCT::template depth<get_sz>(v); }
    static int _dist(node *v, node *u) { return LCT::template _dist<get_sz>(v, u); }

    static void set(node *v, S x) {
        v->_val.x = x;
        update(v);
        expose(v);
    }

    static S get(node *v) {
        expose(v);
        return v->_val.x;
    }

    // vから根までのprod
    static S path_prod(node *v) {
        expose(v);
        return v->_val.xsum;
    }

    static node *path_min_node(node *s, node *t) {
        evert(s);
        expose(t);
        return t->min_node;
    }

    // root -> vのパスで g(root, ... u)がtrueになる最遠のu
    // !g(root)ならnullptr
    template<typename G>
    static node *max_right(node *v, G g) {
        expose(v);
        S sum = e();
        node *u = nullptr;
        node *res = nullptr;
        while (v) {
            u = v;
            LCT::push(v);
            S lsum = (v->lch ? v->lch->_val.xsum : e());
            S tmp = sum;
            if (cmp(lsum, tmp)) tmp = lsum;
            if (cmp(v->_val.x, tmp)) tmp = v->_val.x;
            if (g(tmp)) {
                res = v;
                sum = tmp;
                v = v->rch;
            } else {
                v = v->lch;
            }
        }
        LCT::splay(u);
        return res;
    }
};
#endif