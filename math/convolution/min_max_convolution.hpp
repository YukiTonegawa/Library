#ifndef _MIN_MAX_CONVOLUTION_H_
#define _MIN_MAX_CONVOLUTION_H_
#include <vector>
#include <numeric>
#include <algorithm>
#include "../base/bit_ceil.hpp"

template<typename T>
std::vector<T> min_max_convolution_naive(const std::vector<T> &A, const std::vector<T> &B) {
    int N = A.size(), M = B.size();
    std::vector<T> res(N + M - 1, std::numeric_limits<T>::max());
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            res[i + j] = std::min(res[i + j], std::max(A[i], B[j]));
        }
    }
    return res;
}

// A, Bが昇順ソート済 O(N + M)
template<typename T>
std::vector<T> min_max_convolution_asc_asc(const std::vector<T> &A, const std::vector<T> &B) {
    int N = A.size(), M = B.size();
    std::vector<T> res(N + M, std::numeric_limits<T>::max());
    int i = 0, j = 0;
    while (i < N || j < M) {
        int r = i + j;
        if (j == M || (i < N && A[i] < B[j])) {
            res[r] = std::min(res[r], A[i++]);
        } else {
            res[r] = std::min(res[r], B[j++]);
        }
    }
    for (int k = N + M - 1; k > 0; k--) {
        res[k - 1] = std::min(res[k - 1], res[k]);
    }
    res.pop_back();
    return res;
}

// A, Bが降順ソート済 O(N + M)
template<typename T>
std::vector<T> min_max_convolution_dsc_dsc(const std::vector<T> &A, const std::vector<T> &B) {
    int N = A.size(), M = B.size();
    std::vector<T> res(N + M, std::numeric_limits<T>::max());
    int i = 0, j = 0;
    while (i < N || j < M) {
        int r = i + j;
        assert(0 <= r && r < res.size());
        if (j == M || (i < N && A[i] > B[j])) {
            res[r] = std::min(res[r], A[i++]);
        } else {
            res[r] = std::min(res[r], B[j++]);
        }
    }
    for (int k = 1; k < N + M - 1; k++) {
        res[k] = std::min(res[k], res[k - 1]);
    }
    res.pop_back();
    return res;
}

// Aが昇順ソート済 O(N + MlogN)
template<typename T>
std::vector<T> min_max_convolution_asc(const std::vector<T> &A, const std::vector<T> &B) {
    int N = A.size(), M = B.size(), size = bit_ceil(N + M);
    if (std::max(N, M) <= 20) return min_max_convolution_naive<T>(A, B);
    std::vector<T> tree(2 * size, std::numeric_limits<T>::max());
    std::vector<int> pp(N + M - 1, N);
    for (int j = 0; j < M; j++) {
        int i = std::upper_bound(A.begin(), A.end(), B[j]) - A.begin();
        if (i < N) pp[i + j] = std::min(pp[i + j], i);
        {
            int l = j + size, r = i + j + size;
            while (l < r) {
                if (l & 1) {
                    tree[l] = std::min(tree[l], B[j]);
                    l++;
                }
                if (r & 1) {
                    r--;
                    tree[r] = std::min(tree[r], B[j]);
                }
                l >>= 1;
                r >>= 1;
            }
        }
    }
    for (int i = 1; i < size; i++) {
        tree[i * 2] = std::min(tree[i * 2], tree[i]);
        tree[i * 2 + 1] = std::min(tree[i * 2 + 1], tree[i]);
    }
    for (int i = 0; i < N + M - 1; i++) {
        if (i) pp[i] = std::min(pp[i], pp[i - 1] + 1);
        if (pp[i] < N) tree[size + i] = std::min(tree[size + i], A[pp[i]]);
    }
    return std::vector<T>(tree.begin() + size, tree.begin() + size + N + M - 1);
}

// Aが降順ソート済 O(N + MlogN)
template<typename T>
std::vector<T> min_max_convolution_dsc(const std::vector<T> &A, const std::vector<T> &B) {
    int N = A.size(), M = B.size(), size = bit_ceil(N + M);
    if (std::max(N, M) <= 20) return min_max_convolution_naive<T>(A, B);
    std::vector<T> tree(2 * size, std::numeric_limits<T>::max());
    std::vector<int> pp(N + M - 1, -1);
    for (int j = 0; j < M; j++) {
        int i = std::partition_point(A.begin(), A.end(), [&](T x) { return x > B[j]; }) - A.begin();
        if (i) pp[i + j - 1] = std::max(pp[i + j - 1], i - 1);
        {
            int l = i + j + size, r = j + N + size;
            while (l < r) {
                if (l & 1) {
                    tree[l] = std::min(tree[l], B[j]);
                    l++;
                }
                if (r & 1) {
                    r--;
                    tree[r] = std::min(tree[r], B[j]);
                }
                l >>= 1;
                r >>= 1;
            }
        }
    }
    for (int i = 1; i < size; i++) {
        tree[i * 2] = std::min(tree[i * 2], tree[i]);
        tree[i * 2 + 1] = std::min(tree[i * 2 + 1], tree[i]);
    }
    for (int i = N + M - 2; i >= 0; i--) {
        if (i < N + M - 2) pp[i] = std::max(pp[i], pp[i + 1] - 1);
        if (pp[i] >= 0) tree[size + i] = std::min(tree[size + i], A[pp[i]]);
    }
    return std::vector<T>(tree.begin() + size, tree.begin() + size + N + M - 1);
}
#endif