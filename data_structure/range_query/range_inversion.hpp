#ifndef _RANGE_INVERSION_H_
#define _RANGE_INVERSION_H_
#include <vector>
#include <algorithm>
#include "../ordered_set/fenwick_tree_set.hpp"
#include "../mo/mo_algorithm.hpp"

template<typename T>
struct range_inversion {
  private:
    std::vector<T> V;
    std::vector<std::pair<int, int>> Q;

    struct _mo_st {
        std::vector<int> v;
        fenwick_tree_set s;
        int pop = 0;
        long long sum = 0;
        void add_left(int i) {
            pop++;
            sum += s.rank(v[i]);
            s.insert(v[i]);
        }
        void del_left(int i) {
            pop--;
            sum -= s.rank(v[i]);
            s.erase(v[i]);
        }
        void add_right(int i) {
            sum += pop - s.rank(v[i]);
            s.insert(v[i]);
            pop++;
        }
        void del_right(int i) {
            pop--;
            sum -= pop - s.rank(v[i]);
            s.erase(v[i]);
        }
    };

  public:
    range_inversion() {}
    range_inversion(const std::vector<T> &_V) : V(_V) {}

    void query(int l, int r) {
        Q.push_back({l, r});
    }

    std::vector<long long> solve() {
        _mo_st x;
        int N = V.size();
        std::vector<std::pair<T, int>> tmp;
        for (int i = 0; i < N; i++) tmp.push_back({V[i], i});
        std::sort(tmp.begin(), tmp.end());
        x.v.resize(N);
        x.s = fenwick_tree_set(N);
        for (int i = 0; i < N; i++) {
            int id = tmp[i].second;
            x.v[id] = i;
        }
        mo_algorithm<_mo_st> mo(N, x);
        for (auto [l, r] : Q) mo.add_query(l, r);
        std::vector<long long> res(Q.size());
        for (int i = 0; i < (int)Q.size(); i++) {
            int id = mo.process();
            res[id] = x.sum;
        }
        return res;
    }
};
#endif