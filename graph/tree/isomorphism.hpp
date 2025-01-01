#ifndef _ISOMORPHISM_H_
#define _ISOMORPHISM_H_
#include "../base/csr.hpp"
#include "../../string/base/rolling_hash.hpp"
#include "../../base/unordered_compressor.hpp"

template<int maxN>
std::pair<int, std::vector<int>> isomorphism(int root, const csr<int> &G) {
    using RH = rolling_hash<maxN>;
    unordered_compressor<uint64_t> mp(G.N());
    std::vector<int> ans(G.N());
    auto f = [&](auto &&f, int v, int p) -> RH {
        RH res;
        res = res.add_raw(0, 1 + RH::R());
        for (int i = G.begin(v); i < G.end(v); i++) {
            if (G.elist[i] == p) continue;
            RH x = f(f, G.elist[i], v);
            x = x.add_raw(0, RH::R());
            res = res.mul_raw(x.hash());
        }
        ans[v] = mp.insert(res.hash());
        return res;
    };
    f(f, root, -1);
    return {mp.size(), ans};
}
#endif