#ifndef _LCT_H_
#define _LCT_H_
#include "../../data_structure/tree_base/lct.hpp"

template<typename S>
struct lct_plain {
  public:
    struct T {
        int sz;
        S x;
        T(S _x) : sz(1), x(_x) {}
    };
    using node = lct_node<T>;
  
  private:
    static constexpr void update(node *v) {
        v->_val.sz = 1;
        if (v->lch) v->_val.sz += v->lch->_val.sz;
        if (v->rch) v->_val.sz += v->rch->_val.sz;
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
};
#endif