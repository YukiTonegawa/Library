#ifndef _ALL_APPLY_HEAP_H_
#define _ALL_APPLY_HEAP_H_
#include <numeric>
#include <vector>
#include <cassert>

template<typename Key, typename S, S (*op)(S, S), S (*e)()>
struct all_apply_heap {
    using P = std::pair<Key, S>;
    int n;
    std::vector<P> v;
    std::vector<S> lz;

    void push_down(int k) {
        if (lz[k] != e()) {
            v[k * 2].second = op(v[k * 2].second, lz[k]);
            v[k * 2 + 1].second = op(v[k * 2 + 1].second, lz[k]);
            lz[k * 2] = op(lz[k * 2], lz[k]);
            lz[k * 2 + 1] = op(lz[k * 2 + 1], lz[k]);
            lz[k] = e();
        }
    }

    void rotate_down(int k) {
        while (true) {
            int l = k * 2, r = l + 1;
            if (l > n) break;
            if (r > n) {
                if (v[l].first < v[k].first) {
                    std::swap(v[k], v[l]), k = l;
                } else {
                    break;
                }
            } else {
                if (v[l].first < v[k].first || v[r].first > v[k].first) {
                    if (v[l].first < v[r].first) {
                        std::swap(v[k], v[l]), k = l;
                    } else {
                        std::swap(v[k], v[r]), k = r;
                    }
                } else {
                    break;
                }
            }
        }
    }
    
    void rotate_up(int k) {
        while (k > 1) {
            k >>= 1;
            int l = k * 2, r = l + 1;
            if (r > n) {
                if (v[l].first < v[k].first) {
                    std::swap(v[k], v[l]);
                } else {
                    break;
                }
            } else {
                if (v[l].first < v[k].first || v[r].first < v[k].first) {
                    if (v[l].first < v[r].first) {
                        std::swap(v[k], v[l]);
                    } else {
                        std::swap(v[k], v[r]);
                    }
                } else {
                    break;
                }
            }
        }
    }
  public:
    all_apply_heap(): n(0), v(1), lz(1, e()){}
    
    void push(Key _k, S _v) {
        push(std::make_pair(_k, _v));
    }
    
    void push(P x) {
        int vs = v.size();
        if (++n == vs) {
            v.resize(vs << 1);
            lz.resize(vs << 1, e());
        }
        int msb = 31 - __builtin_clz(n);
        for (int i = msb; i >= 1; i--) push_down(n >> i);
        v[n] = x;
        rotate_up(n);
    }
    
    int size() {
        return n;
    }
    
    int empty() {
        return !n;
    }
    
    P min() {
        assert(n);
        return v[1];
    }
    
    P pop_min() {
        assert(n);
        P res = v[1];
        if (n) {
            S x = e();
            int k = n;
            while (k) {
                k >>= 1;
                x = op(x, lz[k]);
            } 
            v[1] = v[n];
            v[1].second = op(v[1].second, x);
            lz[n] = e();
            rotate_down(1);
        }
        n--;
        return res;
    }

    // 全要素の値にxを作用
    void all_apply(S x) {
        if (n) {
            lz[1] = op(lz[1], x);
            v[1].second = op(v[1].second, x);
        }
    }
};
#endif