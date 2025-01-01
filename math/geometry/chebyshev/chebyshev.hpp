#ifndef _CHEBYSHEV_H_
#define _CHEBYSHEV_H_

#include <array>
template<typename Idx, int Dim>
struct chebyshev_multidim {
    static constexpr int Mdim = Dim;
    static constexpr int Cdim = 1 << (Dim - 1);
    using Mpoint = std::array<Idx, Mdim>;
    using Cpoint = std::array<Idx, Cdim>;

    // √2倍に拡大される
    Cpoint m2c(const Mpoint &p) {
        if (Dim == 1) return p;
        if (Dim == 2) return {p[0] + p[1], p[0] - p[1]};
        if (Dim == 3) return {p[0] + p[1] + p[2], p[0] + p[1] - p[2], p[0] - p[1] + p[2], p[0] - p[1] - p[2]};
        Cpoint res;
        res.fill(p[0]);
        for (int i = 0; i < Cdim; i++) {
            for (int j = 0; j < Mdim - 1; j++) {
                if ((i >> j) & 1) {
                    res[i] -= p[Mdim - 1 - j];
                } else {
                    res[i] += p[Mdim - 1 - j];
                }
            }
        }
        return res;
    }

    std::array<std::pair<Idx, Idx>, Cdim> max_min;
    
    chebyshev_multidim() {
        max_min.fill({std::numeric_limits<Idx>::min() / 2, std::numeric_limits<Idx>::max() / 2});
    }
    
    void add_point(const Cpoint &p) {
        for(int i = 0; i < Cdim; i++){
            max_min[i].first = std::max(max_min[i].first, p[i]);
            max_min[i].second = std::min(max_min[i].second, p[i]);
        }
    }

    Idx max_dist(const Cpoint &p) {
        Idx res = std::numeric_limits<Idx>::min();
        for(int i = 0; i < Cdim; i++) {
            res = std::max(res, abs(max_min[i].first - p[i]));
            res = std::max(res, abs(max_min[i].second - p[i]));
        }
        return res;
    }
};
#endif