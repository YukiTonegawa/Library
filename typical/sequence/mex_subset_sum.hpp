#ifndef _MEX_SUBSET_SUM_H_
#define _MEX_SUBSET_SUM_H_
#include <algorithm>
#include <vector>
// subset_sumで作れない自然数のうち小さい方からK個
// O(N^2Klog(NK))
std::vector<uint64_t> mex_subset_sum(std::vector<uint64_t> X, int K) {
    if (K == 0) return {};
    uint64_t S = 0;
    std::sort(X.begin(), X.end());
    std::vector<uint64_t> res;
    for (uint64_t x : X) {
        if (S < x) {
            for (uint64_t y = S + 1; y < x && res.size() < K; y++) {
                res.push_back(y);
            }
            if (res.size() >= K) {
                res.resize(K);
                return res;
            }
            for (int i = 0; i < res.size(); i++) {
                if (res[i] > S) break;
                res.push_back(res[i] + x);
            }
        } else {
            std::vector<uint64_t> next;
            int i = 0;
            while (i < res.size() && res[i] < x) {
                next.push_back(res[i++]);
                if (next.size() == K) return next;
            }
            while (i < res.size()) {
                uint64_t y = res[i++];
                auto itr = std::lower_bound(res.begin(), res.end(), y - x);
                if (itr != res.end() && *itr == y - x) {
                    next.push_back(y);
                }
            }
            for (int j = 0; j < res.size(); j++) {
                uint64_t y = res[j];
                if (y + x > S) {
                    next.push_back(y + x);
                }
            }
            std::swap(res, next);
        }
        S += x;
    }
    while (res.size() < K) {
        res.push_back(++S);
    }
    res.resize(K);
    return res;
}
#endif