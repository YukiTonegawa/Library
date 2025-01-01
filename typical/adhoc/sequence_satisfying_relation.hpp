#ifndef _SEQUENCE_SATISFYING_RELATION_H_
#define _SEQUENCE_SATISFYING_RELATION_H_
#include <vector>
#include <algorithm>
#include <numeric>

// ><系
struct sequence_satisfying_relation {
    // S, 下限, 上限, 和が最小

    // AGC40A
    // N+1要素の非負数列 与えられる隣接N項の大小関係(> or <)を満たす中で各項が最小
    static std::vector<int> restore_minimum(std::string S) {
        S += '<';
        int N = S.size();
        std::vector<int> f(N, -1);
        for (int i = 0; i < N; i++) {
            if (S[i] == '<' && (i == 0 || S[i - 1] == '>')) {
                f[i] = 0;
                int j = i;
                while (j + 1 < N && S[j + 1] == '<') {
                    f[j + 1] = f[j] + 1;
                    j++;
                }
                j = i;
                while (j > 0 && (j == 1 || S[j - 2] == '>')) {
                    f[j - 1] = f[j] + 1;
                    j--;
                }
            }
        }
        for (int i = 0; i < N; i++) {
            if (f[i] == -1) {
                f[i] = std::max(i ? f[i - 1] : -1, i + 1 < N ? f[i + 1] : -1) + 1;
            }
        }
        return f;
    }

    // AGC53A
    // 大小関係を保ったまま複数の非負整数列に分割, 数列の数が最大になるように分割
    static std::vector<std::vector<int>> decompose_maximum_number(const std::vector<int> &A) {
        int N = A.size(), K = std::numeric_limits<int>::max();
        assert(N >= 2);
        for (int i = 0; i < N - 1; i++) {
            K = std::min(K, abs(A[i] - A[i + 1]));
        }
        std::vector<std::vector<int>> res(K, std::vector<int>(N));
        for (int k = 0; k < K; k++) {
            for (int i = 0; i < N; i++) {
                res[k][i] = (A[i] + k) / K;
            }
        }
        return res;
    }
};
#endif