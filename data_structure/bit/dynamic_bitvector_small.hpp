#ifndef _DYNAMIC_BITVECTOR_SMALL_H_
#define _DYNAMIC_BITVECTOR_SMALL_H_
#include <vector>
#include <array>
#include "select.hpp"

// max_sizeを超えると壊れる
struct dynamic_bitvector_small {
    using Int = uint64_t;
    using Size = uint16_t;
    static constexpr int len_block = 64;
    static constexpr int num_block = 4;
    static constexpr int max_size = len_block * num_block;
    Size size, pop;
    std::array<Int, num_block> x;

    dynamic_bitvector_small() : size(0), pop(0) {
        x.fill(0);
    }

    void insert(int k, bool f) {
        assert(0 <= k && k <= size);
        int block = k / len_block;
        int pos = k % len_block;
        for (int i = num_block - 1; i > block; i--) {
            x[i] <<= 1;
            x[i] |= x[i - 1] >> (len_block - 1);
        }
        Int R = x[block] & (~((Int(1) << pos) - 1));
        x[block] ^= R ^ (R << 1);
        x[block] |= Int(f) << pos;
        size++;
        pop += f;
    }

    bool erase(int k) {
        assert(0 <= k && k < size);
        int block = k / len_block;
        int pos = k % len_block;
        bool res = (x[block] >> pos) & 1;
        pop -= res;
        Int L = x[block] & ((Int(1) << pos) - 1);
        Int R = (x[block] >> 1) & (~((Int(1) << pos) - 1));
        x[block] = L | R;
        if (block + 1 < num_block) x[block] |= (x[block + 1] & 1) << (len_block - 1);
        for (int i = block + 1; i < num_block; i++) {
            x[i] >>= 1;
            if (i + 1 < num_block) x[i] |= (x[i + 1] & 1) << (len_block - 1);
        }
        size--;
        return res;
    }

    void set(int k, bool f) {
        assert(0 <= k && k < size);
        int block = k / len_block;
        int pos = k % len_block;
        bool g = (x[block] >> pos) & 1;
        x[block] ^= Int(f ^ g) << pos;
        pop -= g;
        pop += f;
    }

    bool get(int k) {
        assert(0 <= k && k < size);
        int block = k / len_block;
        int pos = k % len_block;
        return (x[block] >> pos) & 1;
    }

    // [0, r)の1の数
    int rank(int r) {
        assert(0 <= r && r <= size);
        int block = r / len_block;
        int pos = r % len_block;
        int res = 0;
        for (int i = 0; i < block; i++) {
            res += __builtin_popcountll(x[i]);
        }
        if (pos) res += __builtin_popcountll(x[block] & ((Int(1) << pos) - 1));
        return res;
    }

    // k個目(0-indexed)の1の場所 k >= popcountの場合は-1
    int select(int k) {
        for (int i = 0, j = 0; i < num_block; i++, j += len_block) {
            int pop = __builtin_popcountll(x[i]);
            if (k < pop) {
                int ans = j + select_64bit(x[i], k);
                return ans < size ? ans : -1;
            }
            k -= pop;
        }
        return -1;
    }

    int select_zero(int k) {
        for (int i = 0, j = 0; i < num_block; i++, j += len_block) {
            int pop = __builtin_popcountll(~x[i]);
            if (k < pop) {
                int ans = j + select_64bit(~x[i], k);
                return ans < size ? ans : -1;
            }
            k -= pop;
        }
        return -1;
    }

    // これを[0, r)に縮小して[r, size)部分を返す
    dynamic_bitvector_small split(int r) {
        assert(0 <= r && r <= size);
        int block = r / len_block;
        int pos = r % len_block;
        dynamic_bitvector_small res;
        for (int i = block; i < num_block; i++) {
            if (i != block && pos) {
                res.x[i - block - 1] |= x[i] << (len_block - pos);
            }
            res.x[i - block] = x[i] << pos;
        }
        for (int i = 0; i < num_block; i++) {
            res.pop += __builtin_popcountll(res.x[i]);
        }
        res.size = size - r;
        size = r;
        pop -= res.pop;
        return res;
    }

    dynamic_bitvector_small split_half() {
        assert(size == max_size);
        int B = num_block / 2;
        dynamic_bitvector_small res;
        for (int i = B; i < num_block; i++) {
            res.x[i - B] = x[i];
            res.pop += __builtin_popcountll(x[i]);
            x[i] = 0;
        }
        res.size = (num_block - B) * len_block;
        size -= res.size;
        pop -= res.pop;
        return res;
    }
};
#endif