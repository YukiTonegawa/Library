#ifndef _RANDOM_H_
#define _RANDOM_H_
#include <random>

struct random_generator {
  private:

    std::mt19937 mt;
    std::mt19937_64 mt64;

  public:

    random_generator(const int seed = 1234) : mt(seed), mt64(seed) {}

    // [0, 2^b)
    uint32_t small(uint32_t b) {
        static constexpr int BITLEN = 32;
        static uint32_t x = 0;
        static int pos = BITLEN;

        if (pos + b <= BITLEN) {
            pos += b;
        } else {
            pos = b;
            x = mt();
        }
        uint32_t res = x & ((1ULL << b) - 1);
        x >>= b;
        return res;
    }

    // [0, 2^32)
    uint32_t u32() {
        return mt();
    }

    // [0, 2^64)
    uint64_t u64() {
        return mt64();
    }

    // 一様分布
    // https://cpprefjp.github.io/reference/random/uniform_real_distribution.html
    double uniform(double a, double b) {
        std::uniform_real_distribution<double> dist(a, b);
        return dist(mt64);
    }

    // 正規分布
    // https://cpprefjp.github.io/reference/random/normal_distribution.html
    // https://ja.wikipedia.org/wiki/%E6%AD%A3%E8%A6%8F%E5%88%86%E5%B8%83
    // ±var  68.2689492%
    // ±2var 95.4499736%
    // ±3var 99.7300204%
    // ±4var 99.993666%
    double normal(double avg, double var) {
        std::normal_distribution<double> dist(avg, var);
        return dist(mt64);
    }
    
    // 指数分布
    // https://cpprefjp.github.io/reference/random/exponential_distribution.html
    // https://ja.wikipedia.org/wiki/%E6%8C%87%E6%95%B0%E5%88%86%E5%B8%83
    double exponential(double lam) {
        std::exponential_distribution<double> dist(lam);
        return dist(mt64);
    }

    // 二項分布
    // https://cpprefjp.github.io/reference/random/binomial_distribution.html
    // judge_prob(p) をx回行った場合の成功回数
    uint32_t binomial(uint32_t x, double p) {
        std::binomial_distribution<> dist(x, p);
        return dist(mt64);
    }

    // 確率pで1, (1-p)で0
    bool judge_prob(double p) {
        static constexpr double inf = (double)std::numeric_limits<uint32_t>::max();
        return u32() < inf * p;
    }

    // 0 <= l < r <= N を満たすl, rをランダムに選ぶ
    // (速度のために厳密に等確率では無い)
    std::pair<uint32_t, uint32_t> segment(uint32_t N) {
        uint32_t l = u32() % N;
        uint32_t r = u32() % N;
        if (l > r) std::swap(l, r);
        return {l, r + 1};
    }

    // 0 <= l < r <= N を満たすl, rをランダムに選ぶ
    // 距離を固定してから決めるため大きい区間が出やすい
    std::pair<uint32_t, uint32_t> segment_wide(uint32_t N) {
        uint32_t d = u32() % N + 1;
        uint32_t l = u32() % (N - d + 1);
        return {l, l + d};
    }
    
    // [0, N)の順列
    std::vector<uint32_t> permutation(uint32_t N) {
        std::vector<uint32_t> res(N);
        std::iota(res.begin(), res.end(), 0);
        shuffle(res.begin(), res.end());
        return res;
    }

    template<typename Itr>
    void shuffle(Itr begin, Itr end) {
        std::shuffle(begin, end, mt);
    }
} rng;
#endif