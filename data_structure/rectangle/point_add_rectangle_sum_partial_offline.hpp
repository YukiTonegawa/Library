#ifndef _RECTSUM_PARTIAL_OFFLINE_H_
#define _RECTSUM_PARTIAL_OFFLINE_H_
#include <algorithm>
#include <cassert>
#include <functional>
#include <vector>
#include "../fenwick_tree/fenwick_tree.hpp"
#include "../../math/base/bit_ceil.hpp"
#include "../wavelet_matrix/wavelet_matrix_compressed.hpp"

template <class Idx, class S>
struct point_add_rectangle_sum_partial_offline {
  private:
    bool built = false;
    compressor<std::pair<Idx, Idx>> P;
    wavelet_matrix_compressed<Idx> wm;
    std::vector<fenwick_tree<S>> seg;
    std::vector<std::tuple<Idx, Idx, S>> _P;
    
  public:
    point_add_rectangle_sum_partial_offline() {}

    void set_initial_point(Idx x, Idx y, S z) {
        assert(!built);
        _P.push_back({x, y, z});
    }

    void build() {
        assert(!built);
        built = true;
        for (auto [x, y, z] : _P) P.add({x, y});
        std::vector<S> val(P.size(), 0);
        for (auto [x, y, z] : _P) {
            int ord = P.ord({x, y});
            val[ord] += z;
        }
        _P.clear();
        _P.shrink_to_fit();
        std::vector<Idx> Y;
        for (int i = 0; i < P.size(); i++) Y.push_back(P[i].second);
        wm = wavelet_matrix_compressed<Idx>(Y, [&](int h, const std::vector<int> &idx) {
            if (seg.size() <= h) seg.resize(h + 1);
            std::vector<S> tmp(val.size());
            for (int i = 0; i < idx.size(); i++) {
                tmp[i] = val[idx[i]];
            }
            seg[h] = fenwick_tree<S>(tmp);
        });
    }
    
    // A[x][y] <- op(A[x][y], z)
    void apply(Idx x, Idx y, S z) {
        if (!built) build();
        int ord = P.ord({x, y});
        assert(P[ord].first == x && P[ord].second == y);
        wm.access_query(ord, [&](int h, int k) {
            seg[h].apply(k, z);
        });
    }

    // [lx, rx) × [ly, ry)に含まれる点のprod
    S prod(Idx lx, Idx rx, Idx ly, Idx ry) {
        if (!built) build();
        int _lx = P.ord({lx, std::numeric_limits<Idx>::min()});
        int _rx = P.ord({rx, std::numeric_limits<Idx>::min()});
        S res = 0;
        wm.range_freq_query(_lx, _rx, ly, ry, [&](int h, int l, int r) {
            res += seg[h].prod(l, r);
        });
        return res;
    }
};
#endif