#ifndef _CHROMATIC_MATCHING_H_
#define _CHROMATIC_MATCHING_H_
#include <vector>
#include <algorithm>
#include <numeric>
#include <unordered_set>
#include "../../base/unordered_compressor.hpp"

template<typename T>
struct chromatic_matching {

    // A[i] != A[j]なら辺が存在するグラフの最大マッチング 
    // O(N)
    static std::vector<std::pair<int, int>> solve_general(const std::vector<T> &A) {
        int N = A.size();
        std::vector<std::vector<int>> freq(N + 1), elem(N);
        unordered_compressor<T> cmp(N);
        for (int i = 0; i < N; i++) {
            int id = cmp.add(A[i]);
            elem[id].push_back(i);
        }
        for (int i = 0; i < N; i++) {
            freq[elem[i].size()].push_back(i);
        }
        std::vector<std::pair<int, int>> res;
        int pos1 = N, pos2 = N;
        while (true) {
            while (pos1 && freq[pos1].empty()) pos1--;
            while (pos2 && int(freq[pos2].size()) - (pos1 == pos2) == 0) pos2--;
            if (pos2 == 0) break;
            int x = freq[pos1].back();
            freq[pos1].pop_back();
            int y = freq[pos2].back();
            freq[pos2].pop_back();
            res.push_back({elem[x].back(), elem[y].back()});
            elem[x].pop_back();
            elem[y].pop_back();
            freq[pos1 - 1].push_back(x);
            freq[pos2 - 1].push_back(y);
        }
        return res;
    }

    // L[i] != R[j]なら辺が存在する二部グラフの最大マッチング 
    // O(N)
    static std::vector<std::pair<int, int>> solve_bipartite(const std::vector<T> &L, const std::vector<T> &R) {
        int N = L.size(), M = R.size(), K = N + M;
        unordered_compressor<T> cmp(K);
        std::vector<std::vector<int>> freq(K + 1), elemL(K), elemR(K);
        for (int i = 0; i < N; i++) {
            int id = cmp.add(L[i]);
            elemL[id].push_back(i);
        }
        for (int i = 0; i < M; i++) {
            int id = cmp.add(R[i]);
            elemR[id].push_back(i);
        }
        for (int i = 0; i < K; i++) {
            freq[elemL[i].size() + elemR[i].size()].push_back(i);
        }
        {
            int pos_row = K + 1, pos_col = -1;
            while (N != M) {
                while (pos_col == -1) {
                    pos_row--;
                    pos_col = (int)freq[pos_row].size() - 1;
                }
                int id = freq[pos_row][pos_col];
                if (N > M && !elemL[id].empty()) {
                    elemL[id].pop_back();
                    freq[pos_row - 1].push_back(id);
                    N--;
                }
                if (N < M && !elemR[id].empty()) {
                    elemR[id].pop_back();
                    freq[pos_row - 1].push_back(id);
                    M--;
                }
                pos_col--;
            }
        }
        std::vector<int> Lids, Rids;
        for (int i = 0; i < K; i++) {
            if (!elemL[i].empty()) Lids.push_back(i);
            if (!elemR[i].empty()) Rids.push_back(i);
        }
        std::vector<std::pair<int, int>> res;
        {
            int pos = K;
            while (N) {
                while (pos && freq[pos].empty()) pos--;
                int id = freq[pos].back();
                freq[pos].pop_back();
                if (elemL[id].size() + elemR[id].size() != pos) continue;
                if (pos > N) {
                    elemL[id].pop_back();
                    elemR[id].pop_back();
                    freq[pos - 2].push_back(id);
                } else if (!elemL[id].empty()) {
                    int lidx = elemL[id].back();
                    elemL[id].pop_back();
                    auto itr = --Rids.end();
                    while (elemR[*itr].empty()) {
                        Rids.erase(itr);
                        itr = --Rids.end();
                    }
                    if (id == *itr) {
                        itr = --(--Rids.end());
                        while (elemR[*itr].empty()) {
                            Rids.erase(itr);
                            itr = --(--Rids.end());
                        }
                    }
                    int rid = *itr;
                    res.push_back({lidx, elemR[rid].back()});
                    elemR[rid].pop_back();
                    if (elemR[rid].empty()) Rids.erase(itr);
                    freq[pos - 1].push_back(id);
                    freq[elemL[rid].size() + elemR[rid].size()].push_back(rid);
                } else {
                    int ridx = elemR[id].back();
                    elemR[id].pop_back();
                    auto itr = --Lids.end();
                    while (elemL[*itr].empty()) {
                        Lids.erase(itr);
                        itr = --Lids.end();
                    }
                    if (id == *itr) {
                        itr = --(--Lids.end());
                        while (elemL[*itr].empty()) {
                            Lids.erase(itr);
                            itr = --(--Lids.end());
                        }
                    }
                    int lid = *itr;
                    res.push_back({elemL[lid].back(), ridx});
                    elemL[lid].pop_back();
                    if (elemL[lid].empty()) Lids.erase(itr);
                    freq[pos - 1].push_back(id);
                    freq[elemL[lid].size() + elemR[lid].size()].push_back(lid);
                }
                N--;
            }
        }
        return res;
    }
};
#endif