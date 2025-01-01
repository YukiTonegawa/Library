#ifndef _RUN_ENUMERATE_H_
#define _RUN_ENUMERATE_H_
#include <vector>
#include <algorithm>
#include <set>
#include <array>
#include "z_algorithm.hpp"

template<typename T>
void run_enumerate(const std::vector<T> &s, const std::vector<T> &rev, std::vector<std::array<int, 3>> &res, int l = 0, int r = -1) {
    int n = s.size();
    if (r == -1) r = s.size();
    if (r - l < 2) return;
    int mid = (l + r) / 2;
    int llen = mid - l;
    int rlen = r - mid;
    std::vector<T> L(llen), R(rlen), R2(rlen + 1 + r - l), L2(llen + 1 + r - l);
    for (int i = 0; i < llen; i++) L[i] = L2[i] = rev[n - mid + i];
    for (int i = 0; i < rlen; i++) R[i] = R2[i] = s[mid + i];
    L2[llen] = R2[rlen] = std::numeric_limits<T>::max();
    for (int i = 0; i < r - l; i++) {
        R2[rlen + 1 + i] = s[l + i];
        L2[llen + 1 + i] = rev[n - r + i];
    }

    std::vector<int> z1 = z_algorithm<T>(L), z2 = z_algorithm<T>(R2);
    int len = r - l;
    for (int i = 0; i < llen; i++) {
        int x = i, y = z1[i];
        if (!i) x = llen, y = 0;
        int z = z2[len + 1 - x];
        int lidx = mid - x - y, ridx = mid + z;
        if (x > 0 && z > 0 && (y + z) >= x) res.push_back({x, lidx, ridx}); //周期, [l, r)
    }

    z1 = z_algorithm<T>(R), z2 = z_algorithm<T>(L2);
    for (int i = 0; i < rlen; i++) {
        int x = i, y = z1[i];
        if (!i) x = rlen, y = 0;
        int z = z2[len + 1 - x];
        int ridx = n - (n - mid - x - y), lidx = n - (n - mid + z);
        if (x > 0 && z > 0 && (y + z) >= x) res.push_back({x, lidx, ridx}); //周期, [l, r)
    }
    run_enumerate<T>(s, rev, res, l, mid);
    run_enumerate<T>(s, rev, res, mid, r);
}

template<typename T>
std::vector<std::array<int, 3>> run_enumerate(const std::vector<T> &s) {
    using ary = std::array<int, 3>;
    int n = (int)s.size();
    //(t, l, r)は周期tを持つ
    //同じ(l, r)での周期を最小にする
    //同じ(l, 周期)でのrを最大化する
    //同じ(r, 周期)でのlを最小化する
    std::vector<ary> tmp, res;
    std::vector<T> t = s;
    std::reverse(t.begin(), t.end());
    run_enumerate<T>(s, t, tmp);
    std::sort(tmp.begin(), tmp.end(), [&](const ary &x, const ary &y) {
        if (x[0] != y[0]) return x[0] < y[0];
        if (x[1] != y[1]) return x[1] < y[1];
        return x[2] > y[2];
    });

    int l = n, r = 0;
    std::set<std::array<int, 2>> st;
    for (int i = 0; i < tmp.size(); i++) {
        ary v = tmp[i];
        if (i && v[0] != tmp[i - 1][0]) l = n, r = 0;
        if (v[1] >= l && v[2] <= r) continue;
        if (n < v[2]) continue;
        l = std::min(l, v[1]);
        r = std::max(r, v[2]);
        if (st.find({v[1], v[2]}) != st.end()) continue;
        res.push_back(v);
        st.insert({v[1], v[2]});
    }
    return res;
}
#endif