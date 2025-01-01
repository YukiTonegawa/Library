#ifndef _DISCRETE_LOGARITHM_H_
#define _DISCRETE_LOGARITHM_H_
#include <algorithm>
#include <cmath>
#include "../modint/modint32_dynamic.hpp"
#include "../../data_structure/hash_table/hash_map.hpp"

// x^k ≡ yであるような最小のk(存在しない場合-1)
// 0^0 = 1とする
// 素数modである必要はない
int bsgs(int _x, int _y, int mod) {
    using _mint = temporary_modint32;
    _mint::set_mod(mod);
    _mint x(_x), y(_y);
    if (y == 1) return 0;
    if (y == 0) {
        if (x == 0) return _mint::mod() == 1 ? 0 : 1;
        _mint tmp = x;
        for (int i = 2; i <= 64; i++) {
            tmp *= x;
            if (tmp.val() == 0) return i;
        }
        return -1;
    }
    int sq = std::sqrt(_mint::mod()) + 1;
    hash_map<unsigned int, int> table(2 * sq + 1);

    _mint xby = y;
    for (int b = 0; b < sq; b++, xby *= x) {
        table.emplace_replace(xby.val(), b);
    }
    
    _mint xH = x.pow(sq);
    _mint xaH = xH;
    for (int a = 1; a <= sq; a++, xaH *= xH) {
        auto [f, id] = table.find(xaH.val());
        if (!f) continue;
        int res = a * sq - id;
        if (x.pow(res) != y) return -1;
        return res;
    }
    return -1;
}

// 素数m, 数列A, bを与える
// 各a_iに対してb ^ k ≡ a_iを満たすkを返す
std::vector<int> bsgs_many(int b, const std::vector<int> &A, int mod) {
    using _mint = temporary_modint32;
    _mint::set_mod(mod);
    int init_size = 5e6; // mod998244353, |A| = 2e5で設定 メモリ130MB |A|が大きい場合は1e7などにする 
    init_size = std::min(init_size, _mint::mod());
    // 前処理 O(init_size)
    hash_map<unsigned, int> mp(init_size * 2);
    _mint x = 1;
    for (int i = 0; i < init_size; i++) {
        mp.emplace(x.val(), i);
        x *= b;
    }
    std::vector<int> res;
    int imax = ((long long)_mint::mod() + init_size - 1) / init_size;
    _mint y = _mint(b).pow((long long)2 * _mint::mod() - 2 - init_size); // b ^ {-init_size}
    // 計算 O(|A| * mod / init_size)
    for (int a : A) {
        x = a;
        for (int i = 0; i < imax; i++) {
            auto [f, j] = mp.find(x.val());
            if (f) {
                res.push_back(i * init_size + j);
                break;
            }
            x *= y;
        }
    }
    return res;
}

#endif