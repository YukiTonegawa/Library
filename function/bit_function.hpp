#ifndef _BIT_FUNCTION_H_
#define _BIT_FUNCTION_H_
#include <cstdint>

// f(x) := ((x | a) & b) ^ c
struct bit_function {
    using self_t = bit_function;
    uint64_t a, b, c;
    bit_function() : a(0), b(~uint64_t(0)), c(0) {}
    uint64_t get(uint64_t x) const {
        return ((x | a) & b) ^ c;
    }
    // f(g(x))
    static self_t merge(self_t f, self_t g) {
        auto d = ~f.a & f.b & f.c;
        self_t h;
        h.a = ((g.a & f.a) ^ g.c) | f.a | d;
        h.b = (((g.b ^ f.c) | f.a) & f.b) | d;
        h.c = f.c & ~d;
        return h;
    }
};
#endif