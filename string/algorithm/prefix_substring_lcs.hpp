#ifndef _PREFIX_SUBSTRING_LCS_H_
#define _PREFIX_SUBSTRING_LCS_H_
#include <vector>
#include <tuple>
#include <string>
#include <algorithm>
#include "../../data_structure/segment_tree/binary_indexed_tree.hpp"

struct prefix_substring_lcs {
  private:
    int n, m;
    std::vector<int> S, T;
    std::vector<std::tuple<int, int, int, int>> Q;
    
    std::vector<int> solve_inner() {
        std::vector<std::vector<int>> ih(n + 1, std::vector<int>(m + 1)), iv(n + 1, std::vector<int>(m + 1));
        for (int i = 0; i <= m; i++) ih[0][i] = i;
        for (int i = 0; i <= n; i++) iv[i][0] = 0;
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= m; j++) {
                if (S[i - 1] == T[j - 1]) {
                    ih[i][j] = iv[i][j - 1];
                    iv[i][j] = ih[i - 1][j];
                } else {
                    ih[i][j] = std::max(ih[i - 1][j], iv[i][j - 1]);
                    iv[i][j] = std::min(ih[i - 1][j], iv[i][j - 1]);
                }
            }
        }
        std::vector<int> ans(Q.size(), 0);
        std::sort(Q.begin(), Q.end());
        for (int i = 0, j = 0; i <= n; i++) {
            std::vector<std::tuple<int, int, bool, int>> q;
            while (j < Q.size() && std::get<0>(Q[j]) == i) {
                auto [sr, tl, tr, id] = Q[j++];
                q.push_back({tl, id, false, tl});
                q.push_back({tr, id, true, tl});
            }
            std::sort(q.begin(), q.end());
            if (q.empty()) continue;
            binary_indexed_tree<int> bit(m + 1);
            for (int a = 0, b = 0; a <= m; a++) {
                bit.update(ih[i][a], 1);
                while (b < q.size() && std::get<0>(q[b]) == a) {
                    auto [r, id, f, l] = q[b++];
                    if (f) {
                        ans[id] += bit.query(0, l + 1);
                    } else {
                        ans[id] -= bit.query(0, l + 1);
                    }
                }
            }
        }
        return ans;
    }

  public:
    
    prefix_substring_lcs(std::string &_S, std::string &_T): n(_S.size()), m(_T.size()) {
        S.resize(n), T.resize(m);
        for (int i = 0; i < n; i++) S[i] = _S[i];
        for (int i = 0; i < m; i++) T[i] = _T[i];
    }

    template<typename Val>
    prefix_substring_lcs(std::vector<Val> &_S, std::vector<Val> &_T): n(_S.size()), m(_T.size()) {
        std::vector<Val> cmp;
        for (int i = 0; i < n; i++) cmp.push_back(_S[i]);
        for (int i = 0; i < m; i++) cmp.push_back(_T[i]);
        std::sort(cmp.begin(), cmp.end());
        cmp.erase(std::unique(cmp.begin(), cmp.end()), cmp.end());
        S.resize(n), T.resize(m);
        for (int i = 0; i < n; i++) S[i] = std::lower_bound(cmp.begin(), cmp.end(), _S[i]) - cmp.begin();
        for (int i = 0; i < m; i++) T[i] = std::lower_bound(cmp.begin(), cmp.end(), _T[i]) - cmp.begin();
    }

    // Sの[0, sr), Tの[tl, tr)のlcsの長さ
    void query(int sr, int tl, int tr) {
        int id = Q.size();
        Q.push_back({sr, tl, tr, id});
    }

    std::vector<int> solve() {
        return solve_inner();
    }
};
#endif