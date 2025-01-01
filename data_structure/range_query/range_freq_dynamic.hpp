#ifndef _RANGE_FREQ_DYNAMIC_H_
#define _RANGE_FREQ_DYNAMIC_H_
#include <vector>
#include "../../base/unordered_compressor.hpp"
#include "../ordered_set/ordered_set.hpp"

template<typename T>
struct range_freq_dynamic {
  private:
    std::vector<int> ids;
    unordered_compressor<T> cmp;
    std::vector<ordered_set<int>> idx;

  public:
    range_freq_dynamic(const std::vector<T> &v) : ids(v.size()), cmp(v.size()) {
        int N = v.size();
        for (int i = 0; i < N; i++) cmp.add(v[i]);
        int M = cmp.size();
        idx.resize(M);
        std::vector<std::vector<int>> tmp(M);
        for (int i = 0; i < N; i++) {
            int id = cmp.id(v[i]);
            ids[i] = id;
            tmp[id].push_back(i);
        }
        for (int i = 0; i < M; i++) idx[i] = ordered_set<int>(tmp[i]);
    }

    void set(int k, T x) {
        int id = ids[k];
        idx[id].erase(k);
        id = cmp.add(x);
        if (id == idx.size()) idx.push_back({});
        ids[k] = id;
        idx[id].insert(k);
    }

    // [0, r)のxの個数
    int rank(int r, T x) {
        int id = cmp.id(x);
        if (id == -1) return 0;
        return idx[id].rank(r);
    }

    // k番目のxの位置(ない場合-1)
    int select(int k, T x) {
        int id = cmp.id(x);
        if (idx[id].size() <= k) return -1;
        return idx[id].kth_smallest(k).second;
    }
};

#endif