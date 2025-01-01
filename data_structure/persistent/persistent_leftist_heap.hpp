#ifndef _PERSISTENT_LEFTIST_HEAP_H_
#define _PERSISTENT_LEFTIST_HEAP_H_
#include <algorithm>
#include <vector>
#include <cassert>

// std::less 最小値
// std::greater 最大値
template<typename T, typename cmp = std::less<T>>
struct persistent_leftist_heap {
  private:
    using self_t = persistent_leftist_heap<T, cmp>;
    static constexpr uint32_t bit_ch = 28; // 2^28個ノードを作ると壊れるがその前にMLEするのでヨシ!
    static constexpr uint32_t nullid = (1 << bit_ch) - 1;
    static constexpr uint64_t nullch = ((uint64_t)nullid << bit_ch) + nullid;
    struct node {
        uint64_t slr; // {s : 8bit, l : 28bit, r : 28bit}
        T x;
        node(T _x) : slr(nullch), x(_x) {}
        int get_lch() const { return (slr >> bit_ch) & nullid; }
        int get_rch() const { return slr & nullid; }
        int get_s() const { return slr >> (2 * bit_ch); }
        void set_lch(int id) { slr = (slr & (~((uint64_t)nullid << bit_ch))) | ((uint64_t)id << bit_ch); }
        void set_rch(int id) { slr = (slr & (~(uint64_t)nullid)) | id; }
        void set_s(int s) { slr = (slr & nullch) | ((uint64_t)s << (2 * bit_ch)); }
    };
    static std::vector<node> V;
    static int meld(int id1, int id2) {
        if (id1 == nullid) return id2;
        if (id2 == nullid) return id1;
        if (!cmp()(V[id1].x, V[id2].x)) std::swap(id1, id2);
        V.push_back(V[id1]);
        id1 = V.size() - 1;
        int rch = meld(V[id1].get_rch(), id2);
        V[id1].set_rch(rch);
        int lch = V[id1].get_lch();
        rch = V[id1].get_rch();
        int ls = (lch == nullid ? 0 : V[lch].get_s()), rs = V[rch].get_s();
        if (ls < rs) {
            V[id1].set_lch(rch);
            V[id1].set_rch(lch);
        }
        V[id1].set_s(std::min(ls, rs) + 1);
        return id1;
    }
    int rootid, sz;
    persistent_leftist_heap(int _rootid, int _sz) : rootid(_rootid), sz(_sz) {}

  public:
    persistent_leftist_heap() : rootid(nullid), sz(0) {}

    int size() const {
        return sz;
    }

    bool empty() const {
        return rootid == nullid;
    }

    self_t push(T x) const {
        int id2 = V.size();
        V.push_back(node(x));
        return self_t(meld(rootid, id2), sz + 1);
    }

    T top() const {
        assert(!empty());
        return V[rootid].x;
    }

    self_t pop() const {
        assert(!empty());
        return self_t(meld(V[rootid].get_lch(), V[rootid].get_rch()), sz - 1);
    }

    static self_t meld(self_t a, self_t b) {
        return self_t(meld(a.rootid, b.rootid), a.sz + b.sz);
    }

    self_t _left() const {
        if (rootid == nullid) return self_t();
        int id = V[rootid].get_lch();
        return self_t(id, 0);
    }

    self_t _right() const {
        if (rootid == nullid) return self_t();
        int id = V[rootid].get_rch();
        return self_t(id, 0);
    }
};
template<typename T, typename cmp>
std::vector<typename persistent_leftist_heap<T, cmp>::node> persistent_leftist_heap<T, cmp>::V;
#endif