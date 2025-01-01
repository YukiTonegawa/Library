#ifndef _PARALLEL_UNION_FIND_H_
#define _PARALLEL_UNION_FIND_H_
#include <vector>
#include <numeric>
#include <cassert>
#include <map>

struct parallel_union_find {
  private:
    int N, H;
    std::vector<int> sz;

    void push(int h, int l1, int l2) {
        int offset = h * N;
        bool f = unite(offset + l1, offset + l2);
        if (!f || !h) return;
        int shift = 1 << (h - 1);
        push(h - 1, l1, l2);
        push(h - 1, l1 + shift, l2 + shift);
    }

  public:
    parallel_union_find(int N) : N(N), H(0) {
        int M = 1;
        while(M <= N) M <<= 1, H++;
        sz.resize(N * H, -1);
    }

    int find(int a) {
        if (sz[a] < 0) return a;
        return sz[a] = find(sz[a]);
    }

    // unite(l1 + i, l2 + i), 0 <= i < k
    void parallel_unite(int l1, int l2, int k) {
        assert(0 <= k && l1 + k <= N && l2 + k <= N);
        for (int h = H - 1, offset = 0; h >= 0; h--) {
            int w = 1 << h;
            if (k >= w) {
                push(h, l1 + offset, l2 + offset);
                k -= w;
                offset += w;
            }
        }
    }
    
    bool unite(int a, int b) {
        a = find(a);
        b = find(b);
        if(a == b) return false;
        if (sz[a] < sz[b]) std::swap(a, b);
        sz[b] += sz[a];
        sz[a] = b;
        return true;
    }
    
    int size(int a) {
        assert(0 <= a && a < N);
        return -sz[find(a)];
    }
    
    bool same(int a, int b) {
        assert(0 <= a && a < N);
        assert(0 <= b && b < N);
        return find(a) == find(b);
    }

    // 内部実装:
    // [l1, N), [l2, N)で2^k連続sameかつ[l1 + 2^k, N), [l2 + 2^k, N)で2^k連続sameでも
    // [l1, N), [l2, N)で2^{k+1}連続sameであるという情報を持っているとは限らない
    // ↑を満たすようにする
    void build_parallel_same() {
        for (int h = 0; h < H - 1; h++) {
            int shift = 1 << h, offset = h * N;
            std::map<std::pair<int, int>, int> cmp;
            for (int i = 0; i < N - shift; i++) {
                int a = find(i + offset);
                int b = find(i + shift + offset);
                auto [itr, f] = cmp.emplace(std::pair<int, int>{a, b}, i);
                if (!f) {
                    int j = itr->second; // [i, i + 2^{h+1}) is same [j, j + 2^{h+1})
                    unite(i + offset + N, j + offset + N);
                }
            }
        }
    }

    // k = 0, 1, ... t - 1についてl1 + kとl2 + kが同じ連結成分であるような最大のt
    // build_parallel_sameを呼び出してから(つまりstatic)でないと壊れる
    int parallel_same(int l1, int l2) {
        assert(0 <= l1 && l1 < N);
        assert(0 <= l2 && l2 < N);
        int h = 0;
        while (h < H && find(l1 + h * N) == find(l2 + h * N)) {
            h++;
        }
        if (h == 0) return 0;
        h--;
        int len = 1 << h;
        for ( ; h >= 0; h--) {
            if (find(l1 + len + h * N) == find(l2 + len + h * N)) {
                len += 1 << h;
            }
        }
        return len;
    }

    // k = 0, 1, ... t - 1についてlとl + kが同じ連結成分であるような最大のt
    // build_parallel_sameを呼び出してから(つまりstatic)でないと壊れる
    int range_same(int l) {
        if (l + 1 == N) return 1;
        return parallel_same(l, l + 1) + 1;
    }
};
#endif