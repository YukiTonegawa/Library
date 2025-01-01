#ifndef _DS_ACCUMULATE_H_
#define _DS_ACCUMULATE_H_
#include "../base/rolling_hash.hpp"
#include "../base/stovector.hpp"

template<size_t MAX_LEN = 2000000>
struct ds_accumulate {
  private:
    using rh = rolling_hash_base<MAX_LEN>;
    using ull = uint64_t;
    using self_t = ds_accumulate<MAX_LEN>;

    int N;
    std::vector<ull> table;

  public:
    ds_accumulate() : N(0), table(0) {}

    template<typename T>
    ds_accumulate(const std::vector<T> &V) : N(V.size()), table(N + 1) {
        table[0] = 0;
        for (int i = 0; i < N; i++) {
            table[i + 1] = rh::concat(i, table[i], rh::to_hash(V[i]));
        }
    }

    int size() const {
        return N;
    }

    // hash[0, r)
    ull hash_range(int r) const {
        return table[r];
    }

    // hash[l, r)
    ull hash_range(int l, int r) const {
        return rh::split(l, table[l], table[r]);
    }

    // lcp(this[l, ...) , S2[L, ...))
    int lcp(int l, const self_t &S2, int L) const {
        int maxlen = std::min(size() - l, S2.size() - L);
        assert(maxlen >= 0);
        int a = 0, b = maxlen + 1;
        while (b - a > 1) {
            int c = (a + b) / 2;
            if (hash_range(l, l + c) == S2.hash_range(L, L + c)) {
                a = c;
            } else {
                b = c;
            }
        }
        return a;
    }
};
#endif