#ifndef _NARROW_RECTANGLES_H_
#define _NARROW_RECTANGLES_H_
#include "../../math/data_structure/slope_trick.hpp"
#include "../../math/data_structure/slope_trick_bbst.hpp"

template<typename Idx, typename T = Idx>
struct narrow_rectangles {
    slope_trick<T> st;
    std::vector<Idx> Len;
    // [l, r]の区間を追加 1動かすのに1コストがかかる
    void add_segment(Idx l, Idx r) {
        assert(l < r);
        Idx len = r - l;
        if (!Len.empty()) st.shift(-len, Len.back());
        st.add_abs(l);
        Len.push_back(len);
    }

    T min_cost() {
        return st.min();
    }
};

// Q回の操作でmin_fxがABQ^3程度になる可能性がある(A : 座標の最大, B : 関数の係数の最大値)
template<typename Idx, typename T>
struct narrow_rectangles_bbst {
    slope_trick_bbst<T> st;
    std::vector<Idx> Len;
    
    // [l, r]の区間を追加, 1動かすのにwコストがかかる
    void add_segment(Idx l, Idx r, T w) {
        assert(l < r);
        Idx len = r - l;
        if (!Len.empty()) st.shift(-len, Len.back());
        st.add_abs(l, w);
        Len.push_back(len);
    }

    // 最後の区間の左端がxである時の最小コスト
    T min_cost_fixed_l(Idx x) {
        return st.get(x);
    }

    // 最後の区間がの左端が[lx, rx]である時の最小コスト
    T min_cost_range_l(Idx lx, Idx rx) {
        assert(lx >= rx);
        return st.range_min(lx, rx + 1);
    }

    T min_cost() {
        return st.min();
    }
};

#endif