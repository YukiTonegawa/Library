#ifndef _FENWICK_TREE_MIN_H_
#define _FENWICK_TREE_MIN_H_
#include <vector>
#include <algorithm>

template<typename Val, Val (*id)()>
struct fenwick_tree_prefix_min {
  private:
    int M;
    std::vector<Val> sum;
  
  public:
    fenwick_tree_prefix_min(int N) : M(N), sum(M + 1, id()) { assert(N > 0); }
    
    void reset() {
        std::fill(sum.begin(), sum.end(), id());
    }

    void apply(int k, Val x) {
        for (int i = k + 1; i <= M; i += (i & (-i))) {
            if (sum[i] > x) sum[i] = x;
            else return;
        }
    }

    // min([0, r))
    Val prod(int r) {
        Val res = id();
        for (int k = r; k > 0; k -= (k & (-k))) res = std::min(res, sum[k]);
        return res;
    }
};

#endif