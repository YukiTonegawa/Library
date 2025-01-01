#ifndef _LCT_PATH_PROD_LAZY_H_
#define _LCT_PATH_PROD_LAZY_H_
#include "../../data_structure/tree_base/lct.hpp"

template<typename S, S (*op)(S, S), S (*e)(), S (*_flip)(S), typename F, S (*mapping)(F, S, int), F (*composition)(F, F), F (*id)()>
struct lct_path_prod_lazy {
  public:
    struct T {
        int sz;
        S x, xsum;
        F lz;
        T(S _x) : sz(1), x(_x), xsum(_x), lz(id()) {}
    };
    using node = lct_node<T>;
  
  private:
    static constexpr void update(node *v) {
        v->_val.sz = 1;
        v->_val.xsum = v->_val.x;
        if (v->lch) {
            v->_val.sz += v->lch->_val.sz;
            v->_val.xsum = op(v->lch->_val.xsum, v->_val.xsum);
        }
        if (v->rch) {
            v->_val.sz += v->rch->_val.sz;
            v->_val.xsum = op(v->_val.xsum, v->rch->_val.xsum);
        }
    }

    static constexpr void all_apply(node *v, F x) {
        v->_val.x = mapping(x, v->_val.x, 1);
        v->_val.xsum = mappung(x, v->_val.xsum, v->_val.sz);
    }
    
    static constexpr void push(node *v) {
        if (v->_val.lz != id()) {
            if (v->lch) all_apply(v->lch, v->_val.lz);
            if (v->rch) all_apply(v->rch, v->_val.lz);
            v->_val.lz = id();
        }
    }

    static constexpr void flip(node *v) {
        v->_val.x = _flip(v->_val.x);
        v->_val.xsum = _flip(v->_val.xsum);
    }
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

    static void path_apply(node *v, F x) {
        expose(v);
        all_apply(v, x);
    }

    // vから根までのprod
    static S path_prod(node *v) {
        expose(v);
        return v->_val.xsum;
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
            S tmp = op(sum, op(lsum, v->_val.x));
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