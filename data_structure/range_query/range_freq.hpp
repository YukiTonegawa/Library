#ifndef _RANGE_FREQ_H_
#define _RANGE_FREQ_H_
#include <vector>
#include <algorithm>
#include "../../base/unordered_compressor.hpp"

template<typename T>
struct range_freq {
  private:
    unordered_compressor<T> cmp;
    std::vector<std::vector<int>> idx;

  public:
    range_freq(const std::vector<T> &v) : cmp(v.size()) {
        int N = v.size();
        for (int i = 0; i < N; i++) cmp.add(v[i]);
        int M = cmp.size();
        idx.resize(M);
        for (int i = 0; i < N; i++) idx[cmp.id(v[i])].push_back(i);
    }

    // [0, r)のxの個数
    int rank(int r, T x) {
        int id = cmp.id(x);
        if (id == -1) return 0;
        return std::lower_bound(idx[id].begin(), idx[id].end(), r) - idx[id].begin();
    }

    // k番目のxの位置(ない場合-1)
    int select(int k, T x) {
        int id = cmp.id(x);
        if (idx[id].size() <= k) return -1;
        return idx[id][k]; 
    }
};

#endif