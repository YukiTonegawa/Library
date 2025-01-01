#ifndef _ROLLING_HASH_H_
#define _ROLLING_HASH_H_
#include <vector>
#include <cstdint>
#include <string>
#include <array>
#include "../../math/base/gcd.hpp"
#include "../../math/base/safe_mod.hpp"
#include "../../math/base/fast_mul_mod/montgomery64.hpp"
#include "../../math/number_theory/random_prime.hpp"

// 常にMAX_LEN文字以内
// MAX_LENごとにmod, rを実行時に決める
template<size_t MAX_LEN = 2000000>
struct rolling_hash_base {
  private:
    static constexpr std::array<uint64_t, MAX_LEN + 1> build_rpow() {
        std::array<uint64_t, MAX_LEN + 1> res;
        res[0] = mr.one();
        for (int i = 1; i <= MAX_LEN; i++) res[i] = mr.mul(res[i - 1], r);
        return res;
    }

    static constexpr std::array<uint64_t, MAX_LEN + 1> build_ripow() {
        std::array<uint64_t, MAX_LEN + 1> res;
        res[0] = mr.one();
        for (int i = 1; i <= MAX_LEN; i++) res[i] = mr.mul(res[i - 1], ri);
        return res;
    }

    static constexpr std::array<uint64_t, MAX_LEN + 1> build_rep() {
        std::array<uint64_t, MAX_LEN + 1> res;
        for (int i = 1; i <= MAX_LEN; i++) res[i] = inv_gcd(mr.sub(rpow[i], 1), _mod).second;
        return res;
    }

  public:
    static uint64_t _mod;
    static montgomery64 mr;
    static uint64_t _r, r, ri;
    static std::array<uint64_t, MAX_LEN + 1> rpow, ripow;

    // 1文字/整数 -> hash
    template<typename T>
    static constexpr uint64_t to_hash(T x) { return safe_mod(uint64_t(x) + 1, _mod); }
    static uint64_t get_mod() { return _mod; }
    static uint64_t get_r() { return _r; }
    static constexpr uint64_t add(uint64_t a, uint64_t b) { return mr.add(a, b); }
    static constexpr uint64_t sub(uint64_t a, uint64_t b) { return mr.sub(a, b); }
    static constexpr uint64_t mul(uint64_t a, uint64_t b) { return mr.mul(a, b); }
    static constexpr uint64_t pow(uint64_t a, uint64_t k) { return mr.pow(a, k); }
    //static constexpr uint64_t div(uint64_t a, uint64_t b) { return mr.div(a, b); }
    // (s文字の空文字) + a
    static constexpr uint64_t insert_null(int s, uint64_t a) { return mr.mul(a, rpow[s]); }
    // a = (s文字の空文字) + A のときにAを返す
    static constexpr uint64_t erase_null(int s, uint64_t a) { return mr.mul(a, ripow[s]); }
    // a + b
    static constexpr uint64_t concat(int len_a, uint64_t a, uint64_t b) {
        return mr.add(a, insert_null(len_a, b));
    }
    // b = a + A のときにAを返す
    static constexpr uint64_t split(int len_a, uint64_t a, uint64_t b) {
        return erase_null(len_a, mr.sub(b, a));
    }
    // a + a + ... a (k個)
    static uint64_t rep(int len_a, uint64_t a, int k) {
        static bool first = true;
        static std::array<uint64_t, MAX_LEN + 1> rep_table;
        if (first) {
            first = false;
            rep_table = build_rep();
        }
        assert(k >= 0);
        if (k == 0) return 0;
        uint64_t p = mr.sub(mr.pow(rpow[len_a], k - 1), 1);
        return mr.mul(a, mr.mul(p, rep_table[len_a]));
    }
};

template<size_t MAX_LEN>
uint64_t rolling_hash_base<MAX_LEN>::_mod = random_prime((1ULL << 63) - (1LL << 50), (1ULL << 63) - 1);
template<size_t MAX_LEN>
montgomery64 rolling_hash_base<MAX_LEN>::mr = montgomery64(rolling_hash_base<MAX_LEN>::_mod);

template<size_t MAX_LEN>
uint64_t rolling_hash_base<MAX_LEN>::_r = random_prime((1ULL << 60), (1ULL << 62) - (1ULL << 50));
template<size_t MAX_LEN>
uint64_t rolling_hash_base<MAX_LEN>::r = rolling_hash_base<MAX_LEN>::mr.generate(_r);
template<size_t MAX_LEN>
uint64_t rolling_hash_base<MAX_LEN>::ri = rolling_hash_base<MAX_LEN>::mr.generate(inv_gcd(_r, _mod).second);
template<size_t MAX_LEN>
std::array<uint64_t, MAX_LEN + 1> rolling_hash_base<MAX_LEN>::rpow = rolling_hash_base<MAX_LEN>::build_rpow();
template<size_t MAX_LEN>
std::array<uint64_t, MAX_LEN + 1> rolling_hash_base<MAX_LEN>::ripow = rolling_hash_base<MAX_LEN>::build_ripow();


template<size_t MAX_LEN = 2000000>
struct rolling_hash {
  private:
    using self_t = rolling_hash<MAX_LEN>;
    rolling_hash(size_t s, uint64_t h) : _size(s), _hash(h) {}
  public:
    using rh = rolling_hash_base<MAX_LEN>;
    size_t _size;
    uint64_t _hash;

    // 0文字
    rolling_hash() : _size(0), _hash(0) {}
    rolling_hash(char x) : rolling_hash((uint64_t)x) {}

    rolling_hash(uint64_t x) {
        assert(0 <= x && x < rh::_mod - 1);
        _size = 1;
        _hash = x + 1;
    }

    // 全要素が [0, mod - 1) 
    // [0, 2^62] ならok
    rolling_hash(const std::vector<uint64_t> &V) {
        _size = V.size();
        _hash = 0;
        for (int i = 0; i < _size; i++) {
            assert(0 <= V[i] && V[i] < rh::_mod - 1);
            _hash = rh::add(_hash, rh::mul(safe_mod(V[i] + 1, rh::_mod), rh::rpow[i]));
        }
    }
    
    rolling_hash(const std::string &S) {
        _size = S.size();
        _hash = 0;
        for (int i = 0; i < _size; i++) {
            _hash = rh::add(_hash, rh::mul(uint64_t(S[i]) + 1, rh::rpow[i]));
        }
    }
    static uint64_t get_mod() { return rh::_mod; }
    static uint64_t get_r() { return rh::_r; }
    int size() const { return _size; }
    uint64_t hash() const { return _hash; }

    // 0文字
    static constexpr self_t zero() { return self_t(); }
    // a + b
    static constexpr self_t concat(const self_t &a, const self_t &b) {
        return self_t(a._size + b._size, rh::concat(a._size, a._hash, b._hash));
    }
    // b = a + A のときにAを返す
    static constexpr self_t split(const self_t &a, const self_t &b) {
        return self_t(b._size - a._size, rh::split(a._size, a._hash, b._hash));
    }
    // a + a + ... a (k個)
    static constexpr self_t rep(const self_t &a, int k) {
        return self_t(a._size * k, rh::rep(a._size, a._hash, k));
    }
    bool operator == (const self_t &r) const { return _hash == r._hash && _size == r._size; }
    self_t operator + (const self_t &r) const { return concat(*this, r); }
    self_t operator += (const self_t &r) { return *this = concat(*this, r); }
    self_t operator - (const self_t &r) const {return split(r, *this); }
    self_t operator -= (const self_t &r) { return *this = split(r, *this); }
};

template<size_t MAX_LEN>
std::ostream &operator << (std::ostream &dest, const rolling_hash<MAX_LEN> &a) {
    dest << a.size() << " " << a.hash();
    return dest;
}

template<size_t MAX_LEN = 2000000>
struct rolling_hash_monoid {
    using S = rolling_hash<MAX_LEN>;
    static constexpr S e() { return S(); }
    static constexpr S op(S a, S b) { return S::concat(a, b); }
};
#endif