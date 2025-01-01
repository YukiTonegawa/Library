#ifndef _PERSISTENT_QUEUE_H_
#define _PERSISTENT_QUEUE_H_
#include <array>
#include <cassert>

template<typename T, int maxQ = 500000>
struct persistent_queue {
  private:
    using Ver = int;
    using self_t = persistent_queue<T, maxQ>;
    struct Rev;
    struct D {
        Ver par, last;
        size_t len, lenL, lenLL;
        T val;
        Rev *rev, *lazy;
    };
    
    static std::array<D, maxQ + 1> data;
    static std::array<Ver, 2 * maxQ + 1> V;
    static size_t Vend, Now;

    struct Rev {
        Ver u;
        size_t Vpos, size;
        Rev(Ver _u, size_t _size) : u(_u), Vpos(V.size()), size(_size) {}

        void build() {
            int ver = data[u].last;
            Vpos = Vend;
            Vend += size;
            for (int i = 0; i < size; i++) {
                V[Vpos + i] = ver;
                ver = data[ver].par;
            }
        }
    };

    void modify() {
        D &d = data[v];
        assert(d.len);
        if (d.lenLL == 0) {
            d.rev = d.lazy;
            d.lenLL = d.lenL;
            if (d.rev->Vpos == V.size()) {
                d.rev->build();
            }
        }
    }
    
    Ver v;
    persistent_queue(int _v) : v(_v) {}
  
  public:
    persistent_queue() : v(0) { 
        D &d = data[0];
        d.len = d.lenL = d.lenLL = 0;
        d.rev = new Rev(0, 0);
    }

    int size() {
        return data[v].len;
    }

    bool empty() {
        return !size();
    }

    self_t push_back(T x) {
        D &d = data[Now];
        D &p = data[v];
        d.last = Now;
        d.val = x;
        d.par = p.last;
        d.len = p.len + 1;
        d.lenL = p.lenL;
        d.lenLL = p.lenLL;
        d.rev = p.rev;
        if (d.len == d.lenL * 2 + 1) {
            d.lenL = d.len;
            d.lazy = new Rev(Now, d.lenL - d.lenLL);
        } else {
            d.lazy = p.lazy;
        }
        return self_t(Now++);
    }

    self_t pop_front() {
        assert(data[v].len);
        modify();
        D &d = data[Now];
        D &p = data[v];
        d.last = p.last;
        d.len = p.len - 1;
        d.lenL = p.lenL - 1;
        d.lenLL = p.lenLL - 1;
        d.rev = p.rev;
        if (d.len == d.lenL * 2 + 1) {
            d.lenL = d.len;
            d.lazy = new Rev(Now, d.lenL - d.lenLL);
        } else {
            d.lazy = p.lazy;
        }
        return self_t(Now++);
    }

    T front() {
        assert(data[v].len);
        modify();
        int i = data[v].rev->Vpos + data[v].lenLL - 1;
        return data[V[i]].val;
    }
};

template<typename T, int maxQ>
std::array<typename persistent_queue<T, maxQ>::D, maxQ + 1> persistent_queue<T, maxQ>::data;
template<typename T, int maxQ>
std::array<int, 2 * maxQ + 1> persistent_queue<T, maxQ>::V;
template<typename T, int maxQ>
size_t persistent_queue<T, maxQ>::Vend = 0;
template<typename T, int maxQ>
size_t persistent_queue<T, maxQ>::Now = 1;
#endif