#ifndef _UNORDERED_COMPRESSOR_H_
#define _UNORDERED_COMPRESSOR_H_
#include <random>
#include <vector>
#include <cassert>
#include "../math/base/bit_ceil.hpp"
#include "../data_structure/hash_table/hash_map.hpp"

template<typename INT>
struct unordered_compressor {
    hash_map<INT, int> mp;
    std::vector<INT> rev;

    // maxN種類より多く追加すると壊れる
    unordered_compressor(size_t maxN) : mp(maxN) {}

    int size() const {
        return rev.size();
    }

    // 追加してidを返す (すでにある場合は何もせずそのidを返す)
    int add(INT x) {
        auto [f, id] = mp.emplace(x, (int)rev.size());
        if (f) rev.push_back(x);
        return id;
    }

    // id (存在しない場合 -1)
    int id(INT x) {
        auto [f, id] = mp.find(x);
        return f ? id : -1;
    }

    INT operator [](int i) const { return rev[i]; }
};

#endif