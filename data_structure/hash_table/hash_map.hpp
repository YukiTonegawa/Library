#ifndef _HASH_MAP_H_
#define _HASH_MAP_H_
#include <vector>
#include <cassert>
#include "../../base/hash_fast.hpp"
#include "../../math/base/bit_ceil.hpp"

// 高速化のために以下のような実装
// ・INF<S>とINF<T>を無効な値にする(入れると壊れる)
// ・サイズの縮小を行わなないため要素列挙を行えない 
template<typename S, typename T>
struct hash_map {
  private:
    static constexpr S _NULL = std::numeric_limits<S>::max();
    static constexpr T _ERASED = std::numeric_limits<T>::max();
    int _size_mod; // table.size() - 1
    int _cnt_data; // データサイズ
    std::vector<std::pair<S, T>> table;
    static constexpr double max_ratio = 0.7;
    static constexpr int init_size = 8;

    void expand() {
        _size_mod = _size_mod * 2 + 1;
        std::vector<std::pair<S, T>> tmp(_size_mod + 1, {_NULL, _ERASED});
        std::swap(tmp, table);
        _cnt_data = 0;
        for (auto [key, val] : tmp) {
            if (key != _NULL && val != _ERASED) {
                emplace(key, val);
            }
        }
    }
  
  public:
    hash_map() : _size_mod(init_size - 1), _cnt_data(0), table(_size_mod + 1, {_NULL, _ERASED}) {}
    hash_map(int size) : _size_mod(bit_ceil(std::max(init_size, size)) - 1), _cnt_data(0), table(_size_mod + 1, {_NULL, _ERASED}) {}


    // {追加することができたか、追加後のtable[x]の参照}
    std::pair<bool, T&> emplace(S x, T y) {
        if (max_ratio * _size_mod < _cnt_data) expand();
        int i = hash_fast::hash(x) & _size_mod;
        while (table[i].first != _NULL) {
            if (table[i].first == x) {
                if (table[i].second == _ERASED) {
                    table[i].second = y;
                    return {true, table[i].second};
                } else {
                    return {false, table[i].second};
                }
            }
            i = (i + 1) & _size_mod;
        }
        table[i] = {x, y};
        _cnt_data++;
        return {true, table[i].second};
    }

    // emplaceを行った後にtable[x]をyにする
    // {table[x]が存在しなかったらtrue、追加後のtable[x]の参照}
    std::pair<bool, T&> emplace_replace(S x, T y) {
        if (max_ratio * _size_mod < _cnt_data) expand();
        int i = hash_fast::hash(x) & _size_mod;
        while (table[i].first != _NULL) {
            if (table[i].first == x) {
                bool f = table[i].second == _ERASED;
                table[i].second = y;
                return {f, table[i].second};
            }
            i = (i + 1) & _size_mod;
        }
        table[i] = {x, y};
        _cnt_data++;
        return {true, table[i].second};
    }

    // 削除することができたか
    bool erase(S x) {
        int i = hash_fast::hash(x) & _size_mod;
        while (table[i].first != _NULL) {
            if (table[i].first == x) {
                bool f = table[i].second != _ERASED;
                table[i].second = _NULL;
                return f;
            }
            i = (i + 1) & _size_mod;
        }
        return false;
    }

    // {存在するか、存在する場合その値の参照}
    std::pair<bool, T&> find(S x) {
        int i = hash_fast::hash(x) & _size_mod;
        while (table[i].first != _NULL) {
            if (table[i].first == x) {
                return {table[i].second != _ERASED, table[i].second};
            }
            i = (i + 1) & _size_mod;
        }
        return {false, table[i].second};
    }
};
#endif