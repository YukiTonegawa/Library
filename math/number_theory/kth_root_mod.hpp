#ifndef _KTH_ROOT_MOD_H_
#define _KTH_ROOT_MOD_H_
#include "primitive_root32.hpp"
#include "discrete_logarithm.hpp"
#include "../base/arithmetic_sequence.hpp"
#include "../base/pow_mod.hpp"

// O(sqrt(P))
// X^K ≡ Y mod P (Pが素数)
// 0 <= 値 <= 10^9
// 存在しない場合-1
int kth_root_mod(int K, int Y, int P) {
    if (K == 0) {
        if (Y == 1) return 0;
        return -1;
    }
    if (Y == 0) return 0;
    {
        int g = gcd(P - 1, K);
        if (pow_mod_constexpr(Y, (P - 1) / g, P) != 1) return -1;
    }
    int g = primitive_root32_constexpr(P);
    int q = bsgs(g, Y, P);
    if (q == -1) return -1;
    // g^q ≡ Y mod P
    // X^k ≡ g^q mod P
    // (P - 1)t ≡ -q mod k
    arithmetic_sequence<int> tmp{P - 1, q};
    tmp = tmp.multiple(K);
    if (tmp.a == arithmetic_sequence<int>::inf) {
        return -1;
    } else {
        long long z = ((long long)P - 1) * tmp.b + q;
        z /= K;
        z %= (P - 1);
        return pow_mod_constexpr(g, z, P);
    }
}
#endif