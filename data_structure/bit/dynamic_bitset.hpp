#ifndef _DYNAMIC_BITSET_H_
#define _DYNAMIC_BITSET_H_
#include <vector>
#include "base/iterator_bitset.hpp"
#include "base/shift_bitset.hpp"

// 同じidのbitsetは必ず同じサイズでなければならない
template<int id>
struct dynamic_bitset {
  private:
    using Int = uint64_t;
    static constexpr int len_block = 64;
    static int _size;
    static int _B;
    std::vector<Int> V;
  public:
    
    dynamic_bitset(bool f) {
        if (f) {
            V.resize(_B, ~(Int)0);
            int rem = _size % len_block;
            if (rem) V.back() &= (Int(1) << rem) - 1;
        }
    } 
    
    template<typename T>
    dynamic_bitset(const std::vector<T> &S, T one) {
        assert(S.size() == _size);
        for (int i = 0; i < _size; i++) {
            if (S[i] == one) {
                int b = i / len_block;
                if (b >= V.size()) {
                    V.resize(b + 1, 0);
                }
                V[b] |= Int(1) << (i % len_block);
            }
        }
    }

    static void set_size(int size) {
        _size = size;
        _B = (size + len_block - 1) / len_block;
    }
    
};

#endif