#ifndef _BINARY_HEAP_MAP_H_
#define _BINARY_HEAP_MAP_H_
#include <vector>
#include <cassert>
#include <algorithm>

template<typename Key, typename Val, typename cmp = std::less<Key>>
struct binary_heap_map {
  private:
    using P = std::pair<Key, Val>;
    int N;
    std::vector<P> V;

    void rotate_down(int k) {
        while (true) {
            int l = k * 2 + 1, r = l + 1;
            if (l >= N) return;
            if (r >= N) {
                if (cmp()(V[l].first, V[k].first)) {
                    std::swap(V[k], V[l]);
                }
                return;
            } else {
                if (cmp()(V[l].first, V[k].first) || cmp()(V[r].first, V[k].first)) {
                    if (cmp()(V[l].first, V[r].first)) {
                        std::swap(V[k], V[l]);
                        k = l;
                    } else {
                        std::swap(V[k], V[r]);
                        k = r;
                    }
                }else {
                    return;
                }
            }
        }
    }

    void rotate_up(int k) {
        while (k) {
            k = (k - 1) / 2;
            int l = k * 2 + 1, r = l + 1;
            if (r >= N) {
                if (cmp()(V[l].first, V[k].first)) {
                    std::swap(V[k], V[l]);
                } else {
                    return;
                }
            } else {
                if (cmp()(V[l].first, V[k].first) || cmp()(V[r].first, V[k].first)) {
                    if (cmp()(V[l].first, V[r].first)) {
                        std::swap(V[k], V[l]);
                    } else {
                        std::swap(V[k], V[r]);
                    }
                } else {
                    return;
                }
            }
        }
    }

    void heapify(const std::vector<P> &_v) {
        N = _v.size();
        V = _v;
        for (int i = (N - 1) / 2; i >= 0; i--) {
            rotate_down(i);
        }
    }

  public:
    binary_heap_map(): N(0) {}
    binary_heap_map(const std::vector<P> &_v) { heapify(_v); }
    
    void push(Key x_k, Val x_v) {
        push(std::make_pair(x_k, x_v));
    }

    void push(P x) {
        static constexpr int vector_init_size = 4;
        int vs = V.size();
        if (vs == N) {
            V.resize(!vs ? vector_init_size : vs << 1, x);
        } else {
            V[N] = x;
        }
        rotate_up(N++);
    }

    int size() {
        return N;
    }

    int empty() {
        return !N;
    }

    P min() {
        assert(N);
        return V[0];
    }

    P pop_min() {
        assert(N);
        P res = V[0];
        if (N == 1) {
            N = 0;
        } else {
            V[0] = V[--N];
            rotate_down(0);
        }
        return res;
    }
};
#endif