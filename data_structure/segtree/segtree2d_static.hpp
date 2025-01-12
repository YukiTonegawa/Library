#ifndef _SEGTREE2D_STATIC_H_
#define _SEGTREE2D_STATIC_H_
#include <algorithm>
#include <cassert>
#include <functional>
#include <vector>
#include "../sparse_table/disjoint_sparse_table.hpp"
#include "../../math/base/bit_ceil.hpp"
#include "../wavelet_matrix/wavelet_matrix_compressed.hpp"

template <class Idx, class S, S (*op)(S, S), S (*e)()>
struct segtree2d_static {
  private:
    bool built = false;
    compressor<std::pair<Idx, Idx>> P;
    wavelet_matrix_compressed<Idx> wm;
    std::vector<disjoint_sparse_table_efficient<S, op, e>> st;
    std::vector<std::tuple<Idx, Idx, S>> _P;
    
  public:
    segtree2d_static() {}

    void set_initial_point(Idx x, Idx y, S z) {
        assert(!built);
        _P.push_back({x, y, z});
    }

    void build() {
        assert(!built);
        built = true;
        for (auto [x, y, z] : _P) P.add({x, y});
        std::vector<S> val(P.size(), e());
        for (auto [x, y, z] : _P) {
            int ord = P.ord({x, y});
            val[ord] = op(val[ord], z);
        }
        _P.clear();
        _P.shrink_to_fit();
        std::vector<Idx> Y;
        for (int i = 0; i < P.size(); i++) Y.push_back(P[i].second);
        wm = wavelet_matrix_compressed<Idx>(Y, [&](int h, const std::vector<int> &idx) {
            if (st.size() <= h) st.resize(h + 1);
            std::vector<S> tmp(val.size());
            for (int i = 0; i < idx.size(); i++) {
                tmp[i] = val[idx[i]];
            }
            st[h] = disjoint_sparse_table_efficient<S, op, e>(tmp);
        });
    }

    // [lx, rx) × [ly, ry)に含まれる点のprod
    S prod(Idx lx, Idx rx, Idx ly, Idx ry) {
        if (!built) build();
        int _lx = P.ord({lx, std::numeric_limits<Idx>::min()});
        int _rx = P.ord({rx, std::numeric_limits<Idx>::min()});
        S res = e();
        wm.range_freq_query(_lx, _rx, ly, ry, [&](int h, int l, int r) {
            res = op(res, st[h].prod(l, r));
        });
        return res;
    }
};
template<typename Idx, typename monoid>
using segtree2d_static_monoid = segtree2d_static<Idx, typename monoid::S, monoid::op, monoid::e>;
#endif