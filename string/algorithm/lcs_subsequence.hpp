#ifndef _LCS_H_
#define _LCS_H_
#include <vector>
#include <algorithm>
#include <cassert>
#include <map>

// O(|A||B| / w)
template<typename T>
int lcs(const std::vector<T> &A, const std::vector<T> &B) {
    if (A.size() > B.size()) return lcs(B, A);
    static constexpr int w = 64;
    int N = A.size(), M = (N + w - 1) / w;
    struct Data {
        bool is_sparse;
        std::vector<uint64_t> pos;
    };
    std::map<T, Data> mp;
    for (int i = 0; i < N; i++) {
        T x = A[i];
        auto itr = mp.find(x);
        if (itr == mp.end()) {
            Data d;
            d.pos.push_back(i);
            mp.emplace(x, d);
        } else {
            itr->second.pos.push_back(i);
        }
    }
    const int sparse_threshold = w;
    for (auto itr = mp.begin(); itr != mp.end(); itr++) {
        auto &d = itr->second;
        d.is_sparse = d.pos.size() <= sparse_threshold;
        if (!d.is_sparse) {
            std::vector<uint64_t> bit(M, 0);
            for (int i : d.pos) bit[i / w] |= (i % w);
            std::swap(bit, d.pos);
        }
    }
    std::vector<uint64_t> dp(M, 0);
    for (auto &b : B) {
        auto itr = mp.find(b);
        if (itr == mp.end()) continue;
        auto &d = itr->second;
        if (!d.is_sparse) {
            for (int i = 0, borrow = 0; i < M; i++) {
                uint64_t x = d.pos[i] & ~dp[i];
                int next = dp[i] < x || (dp[i] == x && borrow);
                dp[i] = (dp[i] - x - borrow) & (d.pos[i] | dp[i]);
                borrow = next;
            }
        } else {
            for (int i = 0, j = 0, borrow = 0; i < M; i++) {
                uint64_t bit = 0;
                while (j < d.pos.size() && d.pos[j] < (i + 1) * w) {
                    bit |= 1ULL << (d.pos[j++] - i * w);
                }
                uint64_t x = bit & ~dp[i];
                int next = dp[i] < x || (dp[i] == x && borrow);
                dp[i] = (dp[i] - x - borrow) & (bit | dp[i]);
                borrow = next;
            }
        }
    }
    int res = 0;
    for (auto &i : dp) {
        res += __builtin_popcountll(i);
    }
    return res;
}

// O(|A||B|)
template<typename T>
int lcs_naive(const std::vector<T> &A, const std::vector<T> &B) {
    int N = A.size(), M = B.size();
    std::vector<std::vector<int>> dp(N + 1, std::vector<int>(M + 1, 0));
    for (int i = 0; i <= N; i++) {
        for (int j = 0; j <= M; j++) {
            if (i) dp[i][j] = dp[i - 1][j];
            if (j) dp[i][j] = std::max(dp[i][j], dp[i][j - 1]);
            if (i && j && A[i - 1] == B[j - 1]) dp[i][j] = std::max(dp[i][j], dp[i - 1][j - 1] + 1);
        }
    }
    return dp[N][M];
}

// O(|a||b|)
template<typename T>
std::vector<T> lcs_restore(const std::vector<T> &a, const std::vector<T> &b){
  int n = a.size(), m = b.size();
  std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));
  std::vector<std::vector<std::pair<int, int>>> prev(n + 1, std::vector<std::pair<int, int>>(m + 1, {-1, -1}));
  for(int i = 0; i <= n; i++){
    for(int j = 0; j <= m; j++){
      if(i && dp[i][j] < dp[i - 1][j]){
        dp[i][j] = dp[i - 1][j];
        prev[i][j] = {i - 1, j};
      }
      if(j && dp[i][j] < dp[i][j - 1]){
        dp[i][j] = dp[i][j - 1];
        prev[i][j] = {i, j - 1};
      }
      if(i && j && a[i - 1] == b[j - 1] && dp[i][j] <= dp[i - 1][j - 1]){
        dp[i][j] = dp[i - 1][j - 1] + 1;
        prev[i][j] = {i - 1, j - 1};
      }
    }
  }
  std::vector<T> res;
  int x = dp[n][m], y = n, z = m;
  while(x){
    std::tie(y, z) = prev[y][z];
    if(x != dp[y][z]){
      assert(a[y] == b[z]);
      res.push_back(a[y]);
    }
    x = dp[y][z];
  }
  std::reverse(res.begin(), res.end());
  return res;
}
#endif