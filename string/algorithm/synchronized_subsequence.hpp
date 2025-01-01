#ifndef _SYNCHRONIZED_SUBSEQUENCE_H_
#define _SYNCHRONIZED_SUBSEQUENCE_H_
#include <string>
#include <queue>
#include <vector>
#include <tuple>
#include <algorithm>
#include <cassert>

// 0,1がN個ずつある01列 i番目の0と1で(使う/使わない)が等しい場合の辞書順最大の部分列
std::string synchronized_subsequence(const std::string &S) {
    int M = S.size();
    if (M == 0) return "";
    {
        int zcnt = 0;
        for (char c : S) {
            assert(c == '0' || c == '1');
            zcnt += c == '0';
        }
        assert(zcnt * 2 == M);
    }
    std::queue<std::pair<int, char>> que;
    std::vector<bool> flag10{false};
    std::vector<std::vector<std::tuple<int, int, bool>>> block{1};
    for (int i = 0; i < M; i++) {
        if (que.empty() || que.front().second == S[i]) {
            que.push({i, S[i]});
        } else {
            auto [l, c] = que.front();
            que.pop();
            block.back().push_back({l, i, c == '1'});
            if (que.empty()) {
                auto [_l, _r, f] = block.back()[0];
                flag10.back() = f;
                flag10.push_back(0);
                block.push_back({});
            }
        }
    }
    int last10 = -1;
    for (int i = 0; i < flag10.size(); i++) {
        if (flag10[i]) last10 = i;
    }
    std::string suf = "";
    // 末尾の01連打
    {
        std::vector<int> jump(M, -1);
        for (int i = last10 + 1; i < flag10.size(); i++) {
            for (auto [l, r, f] : block[i]) {
                jump[l] = r;
            }
        }
        std::vector<int> dp(M, -1);
        dp[0] = 0;
        for (int i = 0; i < M - 1; i++) {
            dp[i + 1] = std::max(dp[i + 1], dp[i]);
            if (jump[i] != -1) dp[jump[i]] = std::max(dp[jump[i]], dp[i] + 1);
        }
        for (int i = 0; i < dp.back(); i++) suf += "01";
    }
    {
        for (int i = last10; i >= 0; i--) {
            if (!flag10[i]) continue;
            int K = block[i].size();
            std::string max_suf = suf;
            std::vector<int> C(M, -1);
            for (int j = K - 1; j >= 0; j--) {
                auto [l, r, f] = block[i][j];
                C[l] = 1;
                C[r] = 0;
                std::string tmp = "";
                for (int k = 0; k < M; k++) {
                    if (C[k] == -1) continue;
                    tmp += (C[k] ? '1' : '0');
                }
                tmp += suf;
                if (tmp > max_suf) max_suf = tmp;
            }
            std::swap(max_suf, suf);
        }
    }
    return suf;
}
#endif
