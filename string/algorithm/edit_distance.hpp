#ifndef _EDIT_DISTANCE_H_
#define _EDIT_DISTANCE_H_
#include <vector>
#include <algorithm>

template<typename T>
int edit_distance(const std::vector<T> &A, const std::vector<T> &B) {
    int N = A.size(), M = B.size();
    std::vector<std::vector<int>> dp(N + 1, std::vector<int>(M + 1));
    for (int i = 0; i <= N; i++) dp[i][0] = i;
    for (int i = 0; i <= M; i++) dp[0][i] = i;
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= M; j++) {
            dp[i][j] = std::min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1] + (A[i - 1] != B[j - 1])});
        }
    }
    return dp[N][M];
}
#endif