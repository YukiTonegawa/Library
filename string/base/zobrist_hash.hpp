#ifndef _ZOBRIST_HASH_H_
#define _ZOBRIST_HASH_H_

#include <vector>
#include <cstdint>
#include "../../util/random_number.hpp"

struct zobrist_hash {
  private:
    static uint64_t r;

    static uint64_t get_hash(uint64_t x) {
        x += r;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        return (x ^ (x >> 31));
    }

    uint64_t _hash;
  
  public:
    
    zobrist_hash() : _hash(0) {}

    zobrist_hash(std::vector<long long> &V) {
        _hash = 0;
        for (long long v : V) {
            _hash ^= get_hash(v);
        }
    }

    uint64_t hash() {
        return _hash;
    }
    
    void insert(long long v) {
        _hash ^= get_hash(v);
    }

    // vが無い状態でeraseすると壊れる
    void erase(long long v) {
        insert(v);
    }

    static zobrist_hash merge(zobrist_hash a, zobrist_hash b) {
        zobrist_hash res;
        res._hash = a._hash ^ b._hash;
        return res;
    }
};

// 0 <= Ai < 2^63なら Ai + x < 2^64
uint64_t zobrist_hash::r = random_number();

#endif