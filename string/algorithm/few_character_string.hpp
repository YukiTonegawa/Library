#ifndef _FEW_CHARACTER_STRING_H_
#define _FEW_CHARACTER_STRING_H_
#include <vector>
#include <string>
#include <cassert>
#include <algorithm>
#include <array>

// 全ての要素が [0, NUM_VAL)
template<int NUM_C = 26>
struct few_character_string {
  private:
    static constexpr int s = 64, sdiv = 6, smod = 63;
    using Z = uint64_t;
    int N;
    std::vector<std::array<int, NUM_C>> B;
    std::vector<std::array<Z, NUM_C>> S;
    std::vector<std::vector<int>> V;

  public:
    few_character_string(){}
    few_character_string(const std::vector<int> &v): N(v.size()) {
        int M = (N + s - 1) / s;
        V.resize(NUM_C, std::vector<int>());
        std::array<int, NUM_C> pop;
        std::array<Z, NUM_C> pop_small;
        pop.fill(0);
        pop_small.fill(0);
        B.resize(M + 1, pop);
        S.resize(M, pop_small);
        for (int i = 0, t = 0, sz = 0; i < N; i++, t++) {
            int x = v[i];
            assert(0 <= x && x < NUM_C);
            V[x].push_back(i);
            pop[x]++, pop_small[x] |= (Z(1) << t);
            if (t == s - 1 || i == N - 1) {
                for (int j = 0; j < NUM_C; j++) {
                    if(j) {
                        pop[j] += pop[j - 1], pop_small[j] |= pop_small[j - 1];
                    }
                    B[sz + 1][j] = pop[j] + B[sz][j];
                    S[sz][j] = pop_small[j];
                }
                pop.fill(0);
                pop_small.fill(0);
                t = -1;
                sz++;
            }
        }
    }

    // count c, i < r, O(1)
    int rank(int r, int c) {
        if (c == 0) return rank_lower(r, c);
        assert(0 <= r && r <= N);
        assert(0 <= c && c < NUM_C);
        int rq = r >> sdiv, rm = r & smod;
        int res = B[rq][c] - B[rq][c - 1];
        if (rm) res += __builtin_popcountll((S[rq][c] ^ S[rq][c - 1]) << (s - rm));
        return res;
    }

    // count [0, c], i < r, O(1)
    int rank_lower(int r, int c) {
        assert(0 <= r && r <= N);
        assert(0 <= c && c < NUM_C);
        int rq = r >> sdiv, rm = r & smod;
        int res = B[rq][c];
        if(rm) res += __builtin_popcountll(S[rq][c] << (s - rm));
        return res;
    }

    // k番目のc, ない場合は-1 O(1)
    int select(int k, int c) {
        assert(0 <= c && c < NUM_C);
        return (k < 0 || V[c].size() <= k) ? -1 : V[c][k];
    }

    // k番目のc以下, ない場合は-1 O(logN)
    int select_lower(int k, int c) {
        assert(0 <= c && c < NUM_C);
        int l = 0, r = N + 1;
        while (r - l > 1) {
            int mid = (l + r) >> 1;
            if (rank_lower(mid, c) <= k) l = mid;
            else r = mid;
        }
        return r == N + 1 ? -1 : l;
    }

    // [i, n)で最も左のc
    int find_next(int i, int c) {
        return select(rank(i, c), c);
    }

    // [0, i]で最も右のc
    int find_prev(int i, int c){
        return select(rank(i + 1, c) - 1, c);
    }
};

#endif