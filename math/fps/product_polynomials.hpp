#ifndef _PRODUCT_POLYNOMIALS_H_
#define _PRODUCT_POLYNOMIALS_H_
#include <vector>
#include <queue>
#include "fps.hpp"

template<typename FPS>
FPS product_polynomials(const std::vector<FPS> &A) {
    int N = A.size();
    if (N == 0) return {1};
    if (N == 1) return A[0];
    std::vector<FPS> tmp;
    using P = std::pair<int, int>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
    for (int i = 0; i < A.size(); i++) pq.push({A[i].size(), i});
    while (pq.size() > 1) {
        auto [asz, a] = pq.top(); pq.pop();
        auto [bsz, b] = pq.top(); pq.pop();
        tmp.push_back((a < N ? A[a] : tmp[a - N]) * (b < N ? A[b] : tmp[b - N]));
        pq.push({asz + bsz, tmp.size() - 1 + N});
    }
    return tmp.back();
}
#endif