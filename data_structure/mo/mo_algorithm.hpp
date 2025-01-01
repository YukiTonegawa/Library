#ifndef _MO_ALGORITHM_H_
#define _MO_ALGORITHM_H_
#include <vector>
#include <math.h>

template<typename mo_struct>
struct mo_algorithm {
  private:
    struct query {
        int i, b, l, r;
        query(int i, int b, int l, int r) : i(i), b(b), l(l), r(r) {}
    };

    std::vector<query> q;
    int n, block_size, pq, pl, pr;
    mo_struct &_st;

    void build() {
        int qsz = std::max(1, (int)q.size());
        block_size = std::max(1, (int)(n * sqrt(1.5 / (double)qsz)));
        for (int i = 0; i < q.size(); i++) q[i].b /= block_size;
        std::sort(q.begin(), q.end(),  [](query &a, query &b){
            if (a.b != b.b) return a.b < b.b;
            return a.b & 1 ? a.r > b.r : a.r < b.r;
        });
    }

  public:
    mo_algorithm(int n, mo_struct &_st) : n(n), pq(0), pl(0), pr(0), _st(_st) {}
    
    int add_query(int l, int r) {
        int qsz = q.size();
        q.emplace_back(query(qsz, l, l, r));
        return qsz;
    }

    // すでに全てのクエリを処理し終わっている場合は-1
    int process() {
        if (pq == 0) build();
        if (pq == q.size()) return -1;
        query &qi = q[pq];
        while (pl > qi.l) _st.add_left(--pl);
        while (pr < qi.r) _st.add_right(pr++);
        while (pl < qi.l) _st.del_left(pl++);
        while (pr > qi.r) _st.del_right(--pr);
        pq++;
        return qi.i;
    }

    // [l, r)が欲しい場合
    int process2() {
        if (pq == 0) build();
        if (pq == q.size()) return -1;
        query &qi = q[pq];
        while (pl > qi.l) _st.add_left(--pl, pr);
        while (pr < qi.r) _st.add_right(pl, pr++);
        while (pl < qi.l) _st.del_left(pl++, pr);
        while (pr > qi.r) _st.del_right(pl, --pr);
        pq++;
        return qi.i;
    }
};

#include "hilbert_curve.hpp"
template<typename mo_struct>
struct mo_algorithm_hilbert {
  private:
    struct query {
        int i, l, r;
        long long ord;
        query(int i, int l, int r) : i(i), l(l), r(r) {}
    };

    std::vector<query> q;
    int n, pq, pl, pr;
    mo_struct &_st;

    void build() {
        for (int i = 0; i < q.size(); i++) q[i].ord = hilbert_curve_order(q[i].l, q[i].r);
        std::sort(q.begin(), q.end(), [](query &a, query &b) { return a.ord < b.ord; });
    }

  public:
    mo_algorithm_hilbert(int n, mo_struct &_st) : n(n), pq(0), pl(0), pr(0), _st(_st) {}
    
    int add_query(int l, int r) {
        int qsz = q.size();
        q.emplace_back(query(qsz, l, r));
        return qsz;
    }

    // すでに全てのクエリを処理し終わっている場合は-1
    int process() {
        if (pq == 0) build();
        if (pq == q.size()) return -1;
        query &qi = q[pq];
        while (pl > qi.l) _st.add_left(--pl);
        while (pr < qi.r) _st.add_right(pr++);
        while (pl < qi.l) _st.del_left(pl++);
        while (pr > qi.r) _st.del_right(--pr);
        pq++;
        return qi.i;
    }

    // [l, r)が欲しい場合
    int process2() {
        if (pq == 0) build();
        if (pq == q.size()) return -1;
        query &qi = q[pq];
        while (pl > qi.l) _st.add_left(--pl, pr);
        while (pr < qi.r) _st.add_right(pl, pr++);
        while (pl < qi.l) _st.del_left(pl++, pr);
        while (pr > qi.r) _st.del_right(pl, --pr);
        pq++;
        return qi.i;
    }
};

#endif