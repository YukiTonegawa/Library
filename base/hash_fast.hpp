#ifndef _HASH_FAST_H_
#define _HASH_FAST_H_
#include <cstdint>
#include <cassert>
#include <random>

uint64_t random64() {
    static std::random_device seed_gen;
    static std::mt19937_64 engine(seed_gen());
    return engine();
}

struct hash_fast {
    static uint64_t r64;
    static uint32_t r32;

    static uint64_t hash_u64(uint64_t x) {
        x += r64;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        return (x ^ (x >> 31));
    }

    static uint32_t hash_u32(uint32_t x) {
        x += r32;
        x = (x ^ (x >> 16)) * 0x7feb352d;
	    x = (x ^ (x >> 15)) * 0x846ca68b;
	    return x ^ (x >> 16);
    }

    // ペアのハッシュ {x, y}と{y, x}は異なるとする
    static uint64_t hash_u64_u64(std::pair<uint64_t, uint64_t> p) {
        return hash_u64(hash_u64(p.first) + p.second);
    }

    // ペアのハッシュ {x, y}と{y, x}は異なるとする
    static uint64_t hash_u32_u32(std::pair<uint32_t, uint32_t> p) {
        return hash_u64(((uint64_t)p.first << 32) + p.second);
    }

    template<typename T>
    static uint64_t hash(T x) { assert(false); }
    static uint64_t hash(uint64_t x) { return hash_u64(x); }
    static uint64_t hash(long long x) { return hash_u64(x); }
    static uint64_t hash(uint32_t x) { return hash_u32(x); }
    static uint64_t hash(int x) { return hash_u32(x); }
    static uint64_t hash(std::pair<uint64_t, uint64_t> x) { return hash_u64_u64(x); }
    static uint64_t hash(std::pair<uint32_t, uint32_t> x) { return hash_u32_u32(x); }
};
uint64_t hash_fast::r64 = random64();
uint32_t hash_fast::r32 = random64();

#endif