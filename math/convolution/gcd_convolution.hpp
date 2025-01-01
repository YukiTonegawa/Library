#ifndef _GCD_CONVOLUTION_H_
#define _GCD_CONVOLUTION_H_

#include <vector>
#include "zeta.hpp"
// 1-indexed, z[k] = ∑{i, j, gcd(i, j)==k} A[i]*B[j]
// gcd(i, j)は最大でmin(|A|, |B|)になる    O(max(NlogN, MlogM))
template<typename T>
std::vector<T> gcd_convolution(const std::vector<T> &A, const std::vector<T> &B) {
    int n = A.size();
    int m = B.size();
    auto Az = zeta_multiple<T, _add_func<T>>(std::move(A));
    auto Bz = zeta_multiple<T, _add_func<T>>(std::move(B));
    std::vector<T> Cz(std::max(n, m), 0);
    for (int i = 0; i < std::min(n, m); i++) Cz[i] = Az[i] * Bz[i];
    return mobius_multiple<T, _sub_func<T>>(Cz);
}
#endif