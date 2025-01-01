#ifndef _PERSISTENT_HASH_MAP_H_
#define _PERSISTENT_HASH_MAP_H_
#include <random>
#include "persistent_array.hpp"

// MAX : 要素数の上限
template<typename T, T (*nullkey)(), typename U, U (*nullval)(), int MAXN>
struct persistent_hash_map {
  private:
    static constexpr auto _null() { return std::pair<T, U>(nullkey(), nullval()); }
    static constexpr int M = MAXN * 2;
    using array_t = persistent_array<std::pair<T, U>, _null, M>;
    using self_t = persistent_hash_map<T, nullkey, U, nullval, MAXN>;
    array_t V;
    size_t sz;
    static size_t r;
    static size_t get_hash(const T &x) {
        size_t y = std::hash<T>()(x);
        y += r;
        y = (y ^ (y >> 16)) * 0x7feb352d;
	    y = (y ^ (y >> 15)) * 0x846ca68b;
	    return y ^ (y >> 16);
    }
    
    persistent_hash_map(array_t _V, size_t _sz) : V(_V), sz(_sz) {}

  public:
    persistent_hash_map() : sz(0) {}

    int size() const {
        return sz;
    }

    // 追加できたか / 新バージョン
    // replace = trueの場合強制的に置き換える(戻り値のboolはfalse)
    template<bool replace = false>
    std::pair<bool, self_t> emplace(T x, U u) const {
        size_t k = get_hash(x) % M;
        for (int i = 0; i < MAXN; i++) {
            auto [y, z] = V.get(k);
            if (y == nullkey()) return {true, self_t(V.set(k, {x, u}), sz + 1)};
            if (x == y) {
                if (z != nullval()) {
                    if constexpr (replace) return {false, self_t(V.set(k, {x, u}), sz + 1)};
                    return {false, *this};
                }
                else return {true, self_t(V.set(k, {x, u}), sz + 1)};
            }
            k = (k + 1) % M;
        }
        assert(false);
    }
    
    // 削除できたか / 新バージョン
    std::pair<bool, self_t> erase(T x) const {
        size_t k = get_hash(x) % M;
        for (int i = 0; i < MAXN; i++) {
            auto [y, z] = V.get(k);
            if (y == nullkey()) return {false, *this};
            if (x == y) {
                if (z != nullval()) return {true, self_t(V.set(k, _null()), sz - 1)};
                else return {false, *this};
            }
            k = (k + 1) % M;
        }
        assert(false);
    }

    std::pair<bool, U> find(T x) const {
        size_t k = get_hash(x) % M;
        for (int i = 0; i < MAXN; i++) {
            auto [y, z] = V.get(k);
            if (y == nullkey()) return {false, nullval()};
            if (x == y) return {z != nullval(), z};
            k = (k + 1) % M;
        }
        assert(false);
    }
};
template<typename T, T (*nullkey)(), typename U, U (*nullval)(), int MAXN>
size_t persistent_hash_map<T, nullkey, U, nullval, MAXN>::r = std::mt19937(std::random_device()())();

#endif