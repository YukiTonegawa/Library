#ifndef _MO_ROLLBACK_H_
#define _MO_ROLLBACK_H_
#include <vector>
#include <cassert>
#include <array>
#include <math.h>
#include <algorithm>

template<typename mo_struct>
struct mo_rollback {
  private:
    struct query {
        int i, b, l, r;
        query(int i, int b, int l, int r) : i(i), b(b), l(l), r(r) {}
    };
    std::vector<query> small_q, large_q;
    const int block_size;
    int pq, pl, pr, n, border;
    bool shot;
    mo_struct &_st;
    void build() {
        std::sort(large_q.begin(), large_q.end(),  [](query &a, query &b) {
            if (a.b != b.b) return a.b < b.b;
            return a.r < b.r;
        });
    }

  public:
    mo_rollback(int n, mo_struct &_st) : block_size(round(sqrt(n))), pq(0), pl(0), pr(0), n(n), shot(false), _st(_st) {}

    int add_query(int l, int r) {
        assert(0 <= l && r <= n && l <= r);
        int qid = large_q.size() + small_q.size();
        if (r - l > block_size) {
            large_q.emplace_back(query(qid, l / block_size,  l, r));
        } else {
            small_q.emplace_back(query(qid, l / block_size,  l, r));
        }
        return qid;
    }

    // すでに全てのクエリを処理し終わっている場合{-1, -1, 0}
    // {クエリの追加された順番, 区間長が√N以上のクエリか}
    std::pair<int, bool> process() {
        if (pq == 0) build();
        if (pq < large_q.size()) {
            query qi = large_q[pq];
            if (!pq || large_q[pq - 1].b < qi.b) {
                _st.reset();
                pr = pl = border = std::min(n, (qi.b + 1) * block_size);
                shot = false;
            }
            pq++;
            if (shot) {
                _st.rollback();
                pl = border;
                shot = false;
            }
            while (pr < qi.r) _st.add_right(pr++);
            _st.snapshot();
            shot = true;
            while (pl > qi.l) _st.add_left(--pl);
            return {qi.i, 1};
        } else if(pq < large_q.size() + small_q.size()) {
            query qi = small_q[pq - (int)large_q.size()];
            pq++;
            _st.naive_solve(qi.l, qi.r);
            return {qi.i, 0};
        } else {
            return {-1, 0};
        }
    }

    // l, rが欲しい場合
    std::pair<int, bool> process2() {
        if (pq == 0) build();
        if (pq < large_q.size()) {
            query qi = large_q[pq];
            if (!pq || large_q[pq - 1].b < qi.b) {
                _st.reset();
                pr = pl = border = std::min(n, (qi.b + 1) * block_size);
                shot = false;
            }
            pq++;
            if(shot) {
                _st.rollback();
                pl = border;
                shot = false;
            }
            while (pr < qi.r) _st.add_right(pl, pr++);
            _st.snapshot();
            shot = true;
            while (pl > qi.l) _st.add_left(--pl, pr);
            return {qi.i, 1};
        } else if(pq < large_q.size() + small_q.size()) {
            query qi = small_q[pq - (int)large_q.size()];
            pq++;
            _st.naive_solve(qi.l, qi.r);
            return {qi.i, 0};
        } else {
            return {-1, 0};
        }
    }
};
#endif