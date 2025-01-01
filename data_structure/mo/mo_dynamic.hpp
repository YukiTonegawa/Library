#ifndef _MO_DYNAMIC_H_
#define _MO_DYNAMIC_H_
#include <vector>

template<typename mo_struct>
struct mo_dynamic {
  private:
    long long block_size;
    struct query {
        int i, l, r, lb, rb, t;
        query(int i, int l, int r, int lb, int rb, int t) : i(i), l(l), r(r), lb(lb), rb(rb), t(t) {}
    };
    int N, pq, pl, pr, pt, max_time = 0;
    mo_struct &_st;
    std::vector<query> q;

  public:
    
    mo_dynamic(int N, mo_struct &st) : N(N), pq(0), pl(0), pr(0), pt(0), _st(st) {}
    
    // t回更新が行われた状態の[l, r)クエリを追加
    int add_query(int l, int r, int t) {
        int qsz = q.size();
        q.push_back(query(qsz, l, r, l, r, t));
        max_time = std::max(max_time, t);
        return qsz;
    }

    void build() {
        int Q = q.size();
        if (Q == 0) return;
        int U = max_time;
        block_size = round(powl((double)N * N * (U + 1) * 3 / Q, 1.0 / 3.0));
        for (int i = 0; i < q.size(); i++) {
            q[i].lb /= block_size;
            q[i].rb /= block_size;
        }
        std::sort(q.begin(), q.end(), [&](query &a, query &b){
            if (a.lb != b.lb) return a.lb < b.lb;
            if (a.rb != b.rb) return a.lb & 1 ? a.rb > b.rb : a.rb < b.rb;
            return a.rb & 1 ? a.t > b.t : a.t < b.t;
        });
    }

    int process() {
        if (pq == 0) build();
        if (pq == q.size()) return -1;
        query &qi = q[pq++];
        while(pt < qi.t) _st.add_time(pl, pr, pt++);
        while(pt > qi.t) _st.sub_time(pl, pr, --pt);
        while(pl > qi.l) _st.add(--pl);
        while(pr < qi.r) _st.add(pr++);
        while(pl < qi.l) _st.del(pl++);
        while(pr > qi.r) _st.del(--pr);
        return qi.i;
    }
};

#endif