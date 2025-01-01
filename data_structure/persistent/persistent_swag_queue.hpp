#ifndef _PERSISTENT_SWAG_QUEUE_H_
#define _PERSISTENT_SWAG_QUEUE_H_
#include <array>
#include <cassert>

template<typename T, T (*op)(T, T), T (*e)(), int maxQ = 500000>
struct persistent_swag_queue {
  private:
    using Ver = int;
    using self_t = persistent_swag_queue<T, op, e, maxQ>;
    struct Rev;
    struct D {
        Ver par, last;
        size_t len, lenL, lenLL;
        T val, rsumL, rsumLL;
        Rev *rev, *lazy;
    };
    
    static std::array<D, maxQ + 1> data;
    static std::array<T, 2 * maxQ + 1> lsum;
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
                lsum[Vpos + i] = op(data[ver].val, (i ? lsum[Vpos + i - 1] : e()));
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
            d.rsumLL = d.rsumL;
            d.rsumL = e();
            if (d.rev->Vpos == V.size()) {
                d.rev->build();
            }
        }
    }
    
    Ver v;
    persistent_swag_queue(int _v) : v(_v) {}
  
  public:
    persistent_swag_queue() : v(0) { 
        D &d = data[0];
        d.len = d.lenL = d.lenLL = 0;
        d.rsumL = d.rsumLL = e();
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
        d = data[v];
        d.par = data[v].last;
        d.last = Now;
        d.val = x;
        d.len++;
        d.rsumLL = op(d.rsumLL, x);
        d.rsumL = op(data[Now].rsumL, x);
        if (d.len == d.lenL * 2 + 1) {
            d.lenL = d.len;
            d.lazy = new Rev(Now, d.lenL - d.lenLL);
            d.rsumL = e();
        }
        return self_t(Now++);
    }

    self_t pop_front() {
        assert(data[v].len);
        modify();
        D &d = data[Now];
        d = data[v];
        d.len--;
        d.lenL--;
        d.lenLL--;
        if (d.len == d.lenL * 2 + 1) {
            d.lenL = d.len;
            d.lazy = new Rev(Now, d.lenL - d.lenLL);
            d.rsumL = e();
        }
        return self_t(Now++);
    }

    T front() {
        modify();
        int i = data[v].rev->Vpos + data[v].lenLL - 1;
        return data[V[i]].val;
    }

    T left_prod() {
        if (empty()) return e();
        modify();
        int i = data[v].rev->Vpos + data[v].lenLL - 1;
        return lsum[i];
    }

    T right_prod() {
        if (empty()) return e();
        modify();
        return data[v].rsumLL;
    }

    T all_prod() {
        if (empty()) return e();
        modify();
        int i = data[v].rev->Vpos + data[v].lenLL - 1;
        return op(lsum[i], data[v].rsumLL);
    }
};

template<typename T, T (*op)(T, T), T (*e)(), int maxQ>
std::array<typename persistent_swag_queue<T, op, e, maxQ>::D, maxQ + 1> persistent_swag_queue<T, op, e, maxQ>::data;
template<typename T, T (*op)(T, T), T (*e)(), int maxQ>
std::array<T, 2 * maxQ + 1> persistent_swag_queue<T, op, e, maxQ>::lsum;
template<typename T, T (*op)(T, T), T (*e)(), int maxQ>
std::array<int, 2 * maxQ + 1> persistent_swag_queue<T, op, e, maxQ>::V;
template<typename T, T (*op)(T, T), T (*e)(), int maxQ>
size_t persistent_swag_queue<T, op, e, maxQ>::Vend = 0;
template<typename T, T (*op)(T, T), T (*e)(), int maxQ>
size_t persistent_swag_queue<T, op, e, maxQ>::Now = 1;
#endif