#ifndef _COMPRESSOR_H_
#define _COMPRESSOR_H_
#include <vector>
#include <cassert>
#include <algorithm>

// unique = trueなら通常の座圧
//          falseならstd::uniqueで重複要素を消さない
template<typename T, bool unique = true>
struct compressor {
  private:
    bool built = false;
    std::vector<T> V;

    void build() {
        assert(!built);
        built = true;
        std::sort(V.begin(), V.end());
        if (unique) {
            V.erase(std::unique(V.begin(), V.end()), V.end());
        }
    }

  public:
    using Itr = typename std::vector<T>::iterator;
    Itr begin() { return V.begin(); }
    Itr end() { return V.end(); }

    compressor() {}

    // xを追加
    void add(T x) {
        assert(!built);
        V.push_back(x);
    }

    // イテレーターで範囲を追加
    template<typename I>
    void add(I _begin, I _end) {
        assert(!built);
        while (_begin != _end) {
            V.push_back(T(*_begin));
            _begin++;
        }
    }
    
    int size() {
        if (!built) build();
        return V.size();
    }

    // 存在するか
    bool exist(T x) {
        if (!built) build();
        auto itr = lb(x);
        return itr != end() && *itr == x;
    }

    // lower_bound
    Itr lb(T x) {
        if (!built) build();
        return std::lower_bound(V.begin(), V.end(), x);
    }
    
    // upper_bound
    Itr ub(T x) {
        if (!built) build();
        return std::upper_bound(V.begin(), V.end(), x);
    }

    // x未満の数 (unique = trueかつxが存在するなら、何番目に小さいか)
    int ord(T x) {
        if (!built) build();
        return lb(x) - begin();
    }

    // k番目に小さい要素
    T kth(int k) {
        if (!built) build();
        assert(0 <= k && k < V.size());
        return V[k];
    }
    T operator [] (int k) { return kth(k); }
};
#endif