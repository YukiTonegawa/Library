#ifndef _TWO_SAT_TYPICAL_H_
#define _TWO_SAT_TYPICAL_H_
#include "../../math/base/bit_ceil.hpp"
#include "../../math/data_structure/two_sat.hpp"
#include "../../base/unordered_compressor.hpp"
#include "../..//base/compressor.hpp"

// 条件 : xiかyiのどちらかを選ぶ がN個与えられる. 同じ値を2回以上選ばない選択が可能か O(N)
template<typename T>
std::pair<bool, std::vector<bool>> two_sat_problem1(const std::vector<std::pair<T, T>> &_Q) {
    unordered_compressor<T> cmp;
    std::vector<std::pair<int, int>> Q;
    for (auto [x, y] : _Q) {
        int id1 = cmp.add(x);
        int id2 = cmp.add(y);
        Q.push_back({id1, id2});
    }
    int N = Q.size(), M = cmp.size();
    std::vector<std::vector<std::pair<int, bool>>> C(M);
    for (int i = 0; i < N; i++) {
        auto [x, y] = Q[i];
        C[x].push_back({i, 0});
        C[y].push_back({i, 1});
    }
    std::vector<std::pair<int, int>> Qid(N, {-1, -1});
    two_sat sat(4 * N);
    for (int i = 0, vid = 0; i < M; i++) {
        for (int j = 0; j < C[i].size(); j++) {
            auto [qid, lr] = C[i][j];
            if (!lr) Qid[qid].first = vid + 1;
            else Qid[qid].second = vid + 1;
            sat.add_clause_or(vid, 0, vid + 1, 0);
            if (j) {
                sat.add_clause_or(vid - 2, 1, vid, 0);
                sat.add_clause_or(vid - 2, 1, vid + 1, 0);
            }
            vid += 2;
        }
    }
    for (auto [x, y] : Qid) {
        sat.add_clause_or(x, 1, y, 1);
    }
    bool ok = sat.satisfiable();
    if (!ok) return {false, {}};
    std::vector<bool> res(N);
    auto f = sat.answer();
    for (int i = 0; i < N; i++) {
        auto [x, y] = Qid[i];
        res[i] = f[y];
    }
    return {true, res};
}

// 条件 : [ai, bi)を全て選ぶか[ci, di)を全て選ぶ がN個与えられる. 同じ値を2回以上選ばない選択が可能か O(NlogN)
template<typename T>
std::pair<bool, std::vector<bool>> two_sat_problem2(const std::vector<std::tuple<T, T, T, T>> &_Q) {
    int N = _Q.size();
    std::vector<std::tuple<T, T, int>> A;
    std::vector<int> rev(2 * N);
    for (int i = 0; i < N; i++) {
        auto [l1, r1, l2, r2] = _Q[i];
        A.push_back({l1, r1, i * 2});
        A.push_back({l2, r2, i * 2 + 1});
    }
    std::sort(A.begin(), A.end());
    for (int i = 0; i < 2 * N; i++) rev[std::get<2>(A[i])] = i;
    int log = bit_ceil_log(2 * N), size = 1 << log;
    two_sat sat(2 * size);

    for (int i = 0; i < 2 * N; i++) {
        T _r = std::get<1>(A[i]);
        int r = std::partition_point(A.begin(), A.end(), [&](auto a) {return std::get<0>(a) < _r; }) - A.begin();
        r += size;
        int l = i + 1 + size;
        while (l < r) {
            if (l & 1) sat.add_clause_or(i + size, 0, l++, 0);
            if (r & 1) sat.add_clause_or(i + size, 0, --r, 0);
            l >>= 1;
            r >>= 1;
        }
    }
    for (int i = 1; i < size; i++) {
        sat.add_clause_or(i, 1, i * 2, 0);
        sat.add_clause_or(i, 1, i * 2 + 1, 0);
    }

    for (int i = 0; i < N; i++) {
        int a = rev[i * 2] + size, b = rev[i * 2 + 1] + size;
        sat.add_clause_or(a, 1, b, 1);
    }

    bool ok = sat.satisfiable();
    if (!ok) return {false, {}};
    std::vector<bool> res(N);
    auto f = sat.answer();
    for (int i = 0; i < N; i++) {
        res[i] = f[rev[i * 2 + 1] + size];
    }
    return {true, res};
}

// 条件 : [ai, bi)のうち1つでもfなら[ci, di)が全てg O(NlogN)
template<typename T>
std::pair<bool, std::vector<bool>> two_sat_problem4(const std::vector<std::tuple<T, T, bool, T, T, bool>> &_Q) {
    /*
    int N = _Q.size();
    std::vector<std::tuple<T, T, int>> A;
    std::vector<int> rev(2 * N);
    for (int i = 0; i < N; i++) {
        auto [l1, r1, l2, r2] = _Q[i];
        A.push_back({l1, r1, i * 2});
        A.push_back({l2, r2, i * 2 + 1});
    }
    std::sort(A.begin(), A.end());
    for (int i = 0; i < 2 * N; i++) rev[std::get<2>(A[i])] = i;
    int log = bit_ceil_log(2 * N), size = 1 << log;
    two_sat sat(4 * size);

    for (int i = 0; i < 2 * N; i++) {
        T _r = std::get<1>(A[i]);
        int r = std::partition_point(A.begin(), A.end(), [&](auto a) {return std::get<0>(a) < _r; }) - A.begin();
        r += size;
        int l = i + 1 + size;
        while (l < r) {
            if (l & 1) sat.add_clause_or(i + size, 0, l++, 0);
            if (r & 1) sat.add_clause_or(i + size, 0, --r, 0);
            l >>= 1;
            r >>= 1;
        }
    }
    for (int i = 1; i < size; i++) {
        sat.add_clause_or(i, 1, i * 2, 0);
        sat.add_clause_or(i, 1, i * 2 + 1, 0);
    }

    for (int i = 0; i < N; i++) {
        int a = rev[i * 2] + size, b = rev[i * 2 + 1] + size;
        sat.add_clause_or(a, 1, b, 1);
    }

    bool ok = sat.satisfiable();
    if (!ok) return {false, {}};
    std::vector<bool> res(N);
    auto f = sat.answer();
    for (int i = 0; i < N; i++) {
        res[i] = f[rev[i * 2 + 1] + size];
    }
    return {true, res};
    */
}
#endif