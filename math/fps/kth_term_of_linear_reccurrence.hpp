#ifndef _KTH_TERM_H_
#define _KTH_TERM_H_
#include <vector>
#include <cassert>
#include "../base/bit_ceil.hpp"

template<typename FPS>
typename FPS::mint_t kth_coefficient(long long k, FPS &p, FPS &q) {
    using mint = typename FPS::mint_t;
    if (k <= (1 << 15)) return (p.prefix(k + 1) * q.inv(k + 1))[k];
    while (!p.empty() && p.back() == 0) p.pop_back();
    while (!q.empty() && q.back() == 0) q.pop_back();
    int n = p.size(), m = q.size();
    FPS _q(m, 0);
    for (int i = 0; i < m; i++) _q[i] = i & 1 ? -q[i] : q[i];
    int N = std::max(n, m), M = _q.size();
    int z = bit_ceil(N + M - 1);
    p.resize(z), q.resize(z), _q.resize(z);
    butterfly(p), butterfly(q), butterfly(_q);
    mint iz = mint(z).inv();
    for (int i = 0; i < z; i++) {
        p[i] *= _q[i];
        q[i] *= _q[i];
    }
    butterfly_inv(p), butterfly_inv(q);
    p.resize(n + M - 1), q.resize(m + M - 1);
    n = p.size(), m = q.size();
    for (int i = k & 1; i < n; i += 2) p[i / 2] = p[i] * iz;
    for (int i = 0; i < m; i += 2) q[i / 2] = q[i] * iz;
    p.resize((n + 1) / 2);
    q.resize((m + 1) / 2);
    return kth_coefficient<FPS>(k / 2, p, q);
}

// a := 初期値, a[0, d)
// c := (a[i] = ∑{0 <= j < d} a[i - 1 - j] * c[j]を満たす)
template<typename FPS>
typename FPS::mint_t kth_term_of_linear_reccurrence(long long k, const FPS &a, const FPS &c) {
    int d = c.size();
    assert(a.size() == d);
    if (k < d) return a[k];
    FPS Q(d + 1);
    Q[0] = 1;
    for (int i = 0; i < d; i++) Q[i + 1] = -c[i];
    FPS P = Q * a;
    P.resize(d);
    return kth_coefficient<FPS>(k, P, Q);
}
#endif