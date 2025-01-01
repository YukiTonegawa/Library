#ifndef _PERSITENT_STACK_H_
#define _PERSITENT_STACK_H_
#include <vector>
#include <cassert>

template<typename T>
struct persistent_stack {
  private:
    using self_t = persistent_stack<T>;
    static constexpr int nullid = -1;
    struct node {
        int len, par;
        T val;
        node(T _val) : len(1), par(nullid), val(_val) {}
    };
    static std::vector<node> V;
    int backid;
    persistent_stack(int id) : backid(id) {}

  public:
    persistent_stack() : backid(nullid) {}

    int size() const {
        return (backid == nullid ? 0 : V[backid].len);
    }

    bool empty() const {
        return backid == nullid;
    }

    self_t push_back(T x) const {
        int id = V.size();
        V.push_back(node(x));
        if (backid == nullid) return self_t(id);
        V[id].par = backid;
        V[id].len = V[backid].len + 1;
        return self_t(id);
    }

    T back() const {
        assert(backid != nullid);
        return V[backid].val;
    }

    self_t pop_back() const {
        assert(backid != nullid);
        if (V[backid].len == 1) return self_t(nullid);
        return self_t(V[backid].par);
    }
};

template<typename T>
std::vector<typename persistent_stack<T>::node> persistent_stack<T>::V;
#endif