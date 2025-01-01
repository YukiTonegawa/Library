#ifndef _HASH_SET_H_
#define _HASH_SET_H_
#include <vector>
#include <cassert>
#include "../../base/hash_fast.hpp"
#include "../../math/base/bit_ceil.hpp"

// 高速化のために以下のような実装
// ・INF<S>を無効な値にする(入れると壊れる)
// ・サイズの縮小を行わなないため要素列挙を行えない
template<typename S>
struct hash_set {
  private:
    static constexpr S _NULL = std::numeric_limits<S>::max();
    int _size_mod; // table.size() - 1
    int _cnt_data; // データサイズ
    std::vector<std::pair<S, bool>> table;
    static constexpr double max_ratio = 0.7;
    static constexpr int init_size = 8;

    void expand() {
        _size_mod = _size_mod * 2 + 1;
        std::vector<std::pair<S, bool>> tmp(_size_mod + 1, {_NULL, false});
        std::swap(tmp, table);
        _cnt_data = 0;
        for (auto [key, val] : tmp) {
            if (key != _NULL && val) {
                insert(key);
            }
        }
    }

  public:
    hash_set() : _size_mod(init_size - 1), table(_size_mod + 1, {_NULL, false}) {}
    hash_set(int size) : _size_mod(bit_ceil(std::max(init_size, size)) - 1), table(_size_mod + 1, {_NULL, false}) {}

    int size() {
        return _cnt_data;
    }

    // 追加することができたか
    bool insert(S x) {
        if (max_ratio * _size_mod < _cnt_data) expand();
        int i = hash_fast::hash(x) & _size_mod;
        while (table[i].first != _NULL) {
            if (table[i].first == x) {
                bool f = !table[i].second;
                table[i].second = true;
                return f;
            }
            i = (i + 1) & _size_mod;
        }
        table[i] = {x, true};
        _cnt_data++;
        return true;
    }

    // 削除することができたか
    bool erase(S x) {
        int i = hash_fast::hash(x) & _size_mod;
        while (table[i].first != _NULL) {
            if (table[i].first == x) {
                bool f = table[i].second;
                table[i].second = false;
                return f;
            }
            i = (i + 1) & _size_mod;
        }
        return false;
    }

    // {存在するか、存在する場合その値}
    bool find(S x) {
        int i = hash_fast::hash(x) & _size_mod;
        while (table[i].first != _NULL) {
            if (table[i].first == x) {
                return table[i].second;
            }
            i = (i + 1) & _size_mod;
        }
        return false;
    }
};
#endif