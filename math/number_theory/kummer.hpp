#ifndef _KUMMER_H_
#define _KUMMER_H_
#include <vector>

// nCrが素数pで割り切れる回数
int kummer_prime(long long n, long long r, long long p) {
    if (n < r || r < 0) return 0;
    long long m = n - r;
    if (m < r) std::swap(m, r);
    std::vector<long long> D;
    while (m) {
        D.push_back(m % p + r % p);
        m /= p, r /= p;
    }
    int ans = 0, up = 0;
    for (int i = (int)D.size() - 1; i >= 0; i--) {
        if (D[i] + up >= p) {
            up = 1;
            ans++;
        } else {
            up = 0;
        }
    }
    return ans;
}
#endif