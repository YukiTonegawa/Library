#ifndef _PERMUTATION_MATRIX_H_
#define _PERMUTATION_MATRIX_H_
#include <vector>
#include <iostream>
#include <numeric>
#include <cassert>

struct permutation_matrix {
  private:
    using self_t = permutation_matrix;    
    std::vector<int> P;

  public:
    permutation_matrix() {}
    permutation_matrix(const std::vector<int> _P) : P(_P) {}

    int size() const { return P.size(); }

    // f(x)
    int next(int x) const { return P[x]; }

    // f(x)
    int& operator [] (int x) { return P[x]; }

    // g(f) = f(g) = e
    self_t inv() const {
        int N = P.size();
        self_t res;
        res.P.resize(N);
        for (int i = 0; i < N; i++) {
            res.P[next(i)] = i;
        }
        return res;
    }

    // O(N)
    self_t pow(long long K) const {
        int N = size();
        self_t res;
        res.P.resize(N, -1);
        std::vector<int> st;
        auto dfs = [&](auto &&dfs, int v) -> void {
            if (!st.empty() && st[0] == v) return;
            st.push_back(v);
            dfs(dfs, next(v));
        };
        for (int i = 0; i < N; i++) {
            if (res.P[i] != -1) continue;
            st.clear();
            dfs(dfs, i);
            int s = st.size();
            for (int j = 0; j < s; j++) {
                res.P[st[j]] = st[(K + j) % s];
            }
        }
        return res;
    }

    // g(f)
    static self_t composition(const self_t &g, const self_t &f) {
        assert(g.size() == f.size());
        self_t res = f;
        for (int i = 0; i < res.size(); i++) {
            res[i] = g.next(res[i]);
        }
        return res;
    }

    static self_t e(int N) {
        self_t res;
        res.P.resize(N);
        std::iota(res.P.begin(), res.P.end(), 0);
        return res;
    }
};

std::ostream &operator << (std::ostream &dest, const permutation_matrix &A) {
    int N = A.size();
    if (!N) return dest;
    for (int i = 0; i < N - 1; i++) dest << A.next(i) << ' ';
    dest << A.next(N - 1);
    return dest;
}

#endif