#ifndef _CUTSET_STRUCTURE_H_
#define _CUTSET_STRUCTURE_H_
#include <algorithm>
#include <vector>
#include <unordered_set>
#include <random>

struct cutset_structure {
  private:
    // P(success) >= 1/9, P(all_fail) <= 2^{-0.17M}
    static constexpr int M = 200; // P(all_fail) <= 5.8e-11
    static constexpr long long mask = (1LL << 32) - 1;
    static constexpr long long encode(int a, int b) {
        if (a > b) std::swap(a, b);
        return ((long long)a << 32) + b;
    }
    static constexpr std::pair<int, int> decode(long long x) {
        return {x >> 32, x & mask};
    }
    static bool random01() {
        static std::random_device seed_gen;
        static std::mt19937_64 engine(seed_gen());
        static uint64_t x = 0;
        static int pos = 64;
        if (pos == 64) {
            x = engine();
            pos = 0;
        }
        return (x >> (pos++)) & 1;
    }
    int N;
    struct S {
        int maxlv;
        std::vector<std::vector<long long>> X;
        void new_level() {
            int _N = X.back().size();
            X.push_back(std::vector<long long>(_N, 0));
            maxlv++;
        }
        S(int _N) : maxlv(1), X(1, std::vector<long long>(_N, 0)){}
        void insert(int a, int b) {
            long long x = encode(a, b);
            int lv = 0;
            while (true) {
                X[lv][a] ^= x;
                X[lv][b] ^= x;
                if (!random01()) return;
                if (++lv == maxlv) new_level();
            }
        }
    };
    std::unordered_set<long long> E;
    std::vector<S> G;
  
  public:
    cutset_structure() {}
    cutset_structure(int N) : N(N) {
        for(int i = 0; i < M; i++){
            G.push_back(S(N));
        }
    }

    // 辺a-bを追加する(すでにある場合何もしない)
    // O(M)
    void add_edge(int a, int b) {
        auto [itr, f] = E.insert(encode(a, b));
        if (!f) return;
        for (int i = 0; i < M; i++) {
            G[i].insert(a, b);
        }
    }
    
    // 辺a-bを削除(ない場合何もしない)
    // O(M)
    void erase(int a, int b) {
        bool f = E.erase(encode(a, b));
        if (!f) return;
        for (int i = 0; i < M; i++) {
            G[i].insert(a, b);
        }
    }

    // Tと(全体\T)を繋ぐ辺を探す(ない場合{-1, -1})
    // O(M|T|logE)
    std::pair<int, int> find_edge(const std::vector<int> &T) {
        static std::vector<bool> isT;
        if (isT.empty()) isT.resize(N, 0);
        for (int t : T) isT[t] = 1;
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < G[i].maxlv; j++) {
                long long x = 0;
                for (int v : T) x ^= G[i].X[j][v];
                if (!x) continue;
                if (E.find(x) != E.end()) {
                    auto [a, b] = decode(x);
                    if (isT[a] ^ isT[b]) {
                        for (int t : T) isT[t] = 0;
                        return {a, b};
                    }
                }
            }
        }
        for (int t : T) isT[t] = 0;
        return {-1, -1};
    }
};
#endif