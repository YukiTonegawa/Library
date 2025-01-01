#ifndef _SET_INTERSECTION_H_
#define _SET_INTERSECTION_H_
#include <vector>
#include <cmath>
#include <cassert>
#include <unordered_set>

template<typename T>
struct set_intersection {
    int N, Q, q;
    std::vector<std::unordered_set<T>> S;
    std::vector<int> state; // state[i] := S_iがlargeならlargeになった順番, そうでないなら-1
    std::vector<int> large_id; // large[i] := i番目にlargeになった集合の番号
    std::vector<std::vector<std::unordered_set<T>>> large; // large[i][j] := i番目とj番目のlargeの共通要素の集合

    // 集合の数, 最大追加クエリ数
    set_intersection(int _N, int _maxQ): N(_N), Q(_maxQ), q(std::ceil(std::sqrt(_maxQ))), S(_N), state(_N, -1), large(q, std::vector<std::unordered_set<T>>(q)){}
  
    // S_aにbを追加(すでにある場合は何もしない) O(√Q)
    void insert(int a, T b) {
        assert(0 <= a && a < N);
        if(S[a].find(b) != S[a].end()) return;
        S[a].insert(b);
        if (S[a].size() == q && state[a] == -1) {
            state[a] = large_id.size();
            large_id.push_back(a);
        }
        if (state[a] == -1) return;
        int id_a = state[a];
        for (int i = 0; i < large_id.size(); i++) {
            if (S[large_id[i]].find(b) == S[large_id[i]].end()) continue;
            if (i < id_a) {
                large[i][id_a].insert(b);
            } else {
                large[id_a][i].insert(b);
            }
        }
    }
    
    // S_aからbを削除(ない場合は何もしない) O(√Q)
    void erase(int a, T b) {
        assert(0 <= a && a < N);
        if (S[a].find(b) == S[a].end()) return;
        S[a].erase(b);
        if (state[a] == -1) return;
        int id_a = state[a];
        for (int i = 0; i < large_id.size(); i++) {
            if (i < id_a) {
                large[i][id_a].erase(b);
            } else {
                large[id_a][i].erase(b);
            }
        }
    }

    // S_aとS_bの共通要素のうちいずれかを返す, ない場合はinf O(√Q)
    T get(int a, int b) {
        static constexpr T inf = std::numeric_limits<T>::max();
        assert(0 <= a && a < N);
        assert(0 <= b && b < N);
        if (a == b) return S[a].empty() ? inf : *S[a].begin();
        if (S[a].size() > S[b].size()) std::swap(a, b);
        if (S[a].size() < q) {
            for (T x : S[a]) {
                if (S[b].find(x) != S[b].end()) {
                    return x;
                }
            }
            return inf;
        }
        int id_a = state[a], id_b = state[b];
        if (id_a > id_b) std::swap(id_a, id_b);
        if (large[id_a][id_b].empty()) return inf;
        return *large[id_a][id_b].begin();
    }

    // S_aとS_bの共通要素の数 O(√Q)
    int count(int a, int b) {
        assert(0 <= a && a < N);
        assert(0 <= b && b < N);
        if (a == b) return S[a].size();
        if (S[a].size() > S[b].size()) std::swap(a, b);
        if (S[a].size() < q) {
            int cnt = 0;
            for (T x : S[a]) cnt += (S[b].find(x) != S[b].end());
            return cnt;
        }
        int id_a = state[a], id_b = state[b];
        if (id_a > id_b) std::swap(id_a, id_b);
        return large[id_a][id_b].size();
    }

    // S_aとS_bの共通要素を全列挙 O(√Q + 共通要素数)
    std::vector<T> enumerate(int a, int b) {
        assert(0 <= a && a < N);
        assert(0 <= b && b < N);
        if (a == b) return std::vector<T>(S[a].begin(), S[a].end());
        if (S[a].size() > S[b].size()) std::swap(a, b);
        if (S[a].size() < q) {
            std::vector<T> res;
            for (T x : S[a]) {
                if (S[b].find(x) != S[b].end()) {
                    res.push_back(x);
                }
            }
            return res;
        }
        int id_a = state[a], id_b = state[b];
        if (id_a > id_b) std::swap(id_a, id_b);
        return std::vector<T>(large[id_a][id_b].begin(), large[id_a][id_b].end());
    }
};
#endif