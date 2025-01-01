#ifndef _PERSISTENT_UNION_FIND_H_
#define _PERSISTENT_UNION_FIND_H_
#include "persistent_array.hpp"

// N <= MAXN
template<int MAXN>
struct persistent_union_find {
  private:
    using self_t = persistent_union_find;
    static constexpr int e() { return -1; }
    using array_t = persistent_array<int, e, MAXN + 1>;
    
    std::pair<int, int> get_info(int u) const {
        int s = sz[u];
        if (s < 0) return {u, -s};
        return get_info(s);
    }
    array_t sz;
    persistent_union_find(array_t _sz) : sz(_sz) {} 

  public:
    persistent_union_find() {}

    int find(int u) const {
        return get_info(u).first;
    }

    int size(int u) const {
        return get_info(u).second;
    }

    bool same(int u, int v) const {
        return find(u) == find(v);
    }

    self_t unite(int u, int v) const {
        int usz, vsz;
        std::tie(u, usz) = get_info(u);
        std::tie(v, vsz) = get_info(v);
        if (u == v) return *this;
        if (usz < vsz) std::swap(u, v);
        array_t new_sz = sz;
        new_sz = new_sz.set(v, u);
        new_sz = new_sz.set(u, -(usz + vsz));
        return self_t(new_sz);
    }
};
#endif