#ifndef _DIAMETER_H_
#define _DIAMETER_H_
#include "../base/csr.hpp"

// {直径, s, t}
template<typename edge, typename W = typename edge::W>
struct diameter {
    int _s, _t;
    std::vector<int> par;
    
    std::tuple<W, int, int> operator ()(const csr<edge> &g) {
        int N = g.N();
        par.resize(N);
        auto far = [&](auto &&far, int v, int p) -> std::pair<W, int> {
            W maxw = 0;
            int maxi = v;
            for (int i = g.begin(v); i < g.end(v); i++) {
                int to = g.elist[i];
                if (to == p) continue;
                par[to] = v;
                auto [wi, ti] = far(far, to, v);
                wi += g.elist[i].weight();
                if (maxw <= wi) {
                    maxw = wi;
                    maxi = ti;
                }
            }
            return {maxw, maxi};
        };
        auto [_, s] = far(far, 0, -1);
        auto [d, t] = far(far, s, -1);
        _s = s;
        _t = t;
        return {d, s, t};
    }

    std::vector<int> path() {
        std::vector<int> res{_t};
        int t = _t;
        while (t != _s) {
            t = par[t];
            res.push_back(t);
        }
        std::reverse(res.begin(), res.end());
        return res;
    }
};
#endif