#ifndef _PERSISTENT_MAP_H_
#define _PERSISTENT_MAP_H_
#include <vector>
#include <cassert>
#include <numeric>

template<typename T, typename U>
struct persistent_map {
  private:
    using self_t = persistent_map<T, U>;
    static constexpr uint64_t lench = 28;
    static constexpr uint64_t maskch = (1 << lench) - 1;
    static constexpr uint64_t nullid = maskch;
    struct node {
        T k;
        U s;
        int size;
        uint64_t info; // h : 8bit, lch : 28bit, rch : 28bit
        node(T _k, U _s) : k(_k), s(_s), size(1), info(nullid + (nullid << lench) + (1ULL << (2 * lench))) {}
        int get_lch() const { return (info >> lench) & maskch; }
        int get_rch() const { return info & maskch; }
        int get_h() const { return info >> (2 * lench); }
        std::tuple<int, int, int> get() const { return {info >> (2 * lench), (info >> lench) & maskch, info & maskch}; }
        void set_lch(int lch) { info = (info & ~(maskch << lench)) | ((uint64_t)lch << lench); }
        void set_rch(int rch) { info = (info & ~maskch) | rch;}
        void set_h(int h) { info = (info & (maskch + (maskch << lench))) | ((uint64_t)h << (2 * lench)); }
    };
    static std::vector<node> V;

    static int make_node(T k, U s) {
        V.push_back(node(k, s));
        return (int)V.size() - 1;
    }

    static int copy_node(int id) {
        V.push_back(V[id]);
        return (int)V.size() - 1;
    }

    static void update(int id) {
        auto [h, l, r] = V[id].get();
        V[id].size = 1;
        h = 0;
        if (l != nullid) {
            V[id].size += V[l].size;
            h = std::max(h, V[l].get_h());
        }
        if (r != nullid) {
            V[id].size += V[r].size;
            h = std::max(h, V[r].get_h());
        }
        V[id].set_h(h + 1);
    }

    static int build(const std::vector<std::pair<T, U>> &A, int l, int r) {
        if (l == r) return nullid;
        int m = (l + r) / 2;
        int v = make_node(A[m].first, A[m].second);
        if (l < m) {
            int id = build(A, l, m);
            V[v].set_lch(id);
        }
        if (m + 1 < r) {
            int id = build(A, m + 1, r);
            V[v].set_rch(id);
        }            
        update(v);
        return v;
    }

    static int factor(int v) {
        int lch = V[v].get_lch();
        int rch = V[v].get_rch();
        return (lch == nullid ? 0 : V[lch].get_h()) - (rch == nullid ? 0 : V[rch].get_h());
    }

    static int rotate_right(int v) {
        int vl = V[v].get_lch();
        int vlr = V[vl].get_rch();
        V[v].set_lch(vlr);
        V[vl].set_rch(v);
        update(v);
        update(vl);
        return vl;
    }

    static int rotate_left(int v) {
        int vr = V[v].get_rch();
        int vrl = V[vr].get_lch();
        V[v].set_rch(vrl);
        V[vr].set_lch(v);
        update(v);
        update(vr);
        return vr;
    }

    static int balance_insert(int v) {
        int bf = factor(v);
        if (bf == 2) {
            if (factor(V[v].get_lch()) == -1) {
                int c = rotate_left(V[v].get_lch());
                V[v].set_lch(c);
            }
            return rotate_right(v);
        }else if(bf == -2){
            if (factor(V[v].get_rch()) == 1) {
                int c = rotate_right(V[v].get_rch());
                V[v].set_rch(c);
            }
            return rotate_left(v);
        }
        return v;
    }

    static int balance(int v) {
        int bf = factor(v);
        if (bf == 2) {
            int vl = copy_node(V[v].get_lch());
            V[v].set_lch(vl);
            if (factor(vl) == -1) {
                int vlr = copy_node(V[vl].get_rch());
                V[vl].set_rch(vlr);
                V[v].set_lch(rotate_left(vl));
            }
            return rotate_right(v);
        }else if (bf == -2) {
            int vr = copy_node(V[v].get_rch());
            V[v].set_rch(vr);
            if (factor(vr) == 1) {
                int vrl = copy_node(V[vr].get_lch());
                V[vr].set_lch(vrl);
                V[v].set_rch(rotate_right(vr));
            }
            return rotate_left(v);
        }
        return v;
    }

    static int size(int v) {
        return v == nullid ? 0 : V[v].size;
    }

    static int find(int v, T x) {
        while (v != nullid) {
            if (V[v].k == x) return v;
            if (x < V[v].k) v = V[v].get_lch();
            else v = V[v].get_rch();
        }
        return nullid;
    }

    template<bool replace>
    static int emplace(int v, T x, U s) {
        if (v == nullid) return make_node(x, s);
        if (x == V[v].k) {
            if (!replace) return v;
            v = copy_node(v);
            V[v].s = s;
            return v;
        }
        v = copy_node(v);
        if (x < V[v].k) {
            int id = emplace<replace>(V[v].get_lch(), x, s);
            V[v].set_lch(id);
        } else {
            int id = emplace<replace>(V[v].get_rch(), x, s);
            V[v].set_rch(id);
        }
        update(v);
        return balance_insert(v);
    }

    static int erased_node;
    static int cut_leftmost(int v) {
        v = copy_node(v);
        int L = V[v].get_lch();
        if (L != nullid) {
            int id = cut_leftmost(L);
            V[v].set_lch(id);
            update(v);
            return balance(v);
        }
        erased_node = v;
        return V[v].get_rch();
    }

    static int erase(int v, T x) {
        if (v == nullid) return nullid;
        if (V[v].k == x) {
            int lch = V[v].get_lch();
            if (V[v].get_rch() == nullid) return lch;
            int rch = cut_leftmost(V[v].get_rch());
            V[erased_node].set_lch(lch);
            V[erased_node].set_rch(rch);
            update(erased_node);
            return balance(erased_node);
        }
        v = copy_node(v);
        if (x < V[v].k) {
            int id = erase(V[v].get_lch(), x);
            V[v].set_lch(id);
        } else {
            int id = erase(V[v].get_rch(), x);
            V[v].set_rch(id);
        }
        update(v);
        return balance(v);
    }

    int rootid;
    persistent_map(int _rootid) : rootid(_rootid) {}
    
  public:
    // Aがソート済かつユニーク
    persistent_map(const std::vector<std::pair<T, U>> &A) : rootid(build(A, 0, A.size())) {}
    persistent_map() : rootid(nullid) {}

    int size() const {
        return size(rootid);
    }

    bool empty() const {
        return size(rootid) == 0;
    }

    std::pair<bool, U> find(T x) const {
        int id = find(rootid, x);
        if (id == nullid) return {false, U()};
        return {true, V[id].s};
    }

    self_t emplace(T x, U s) const {
        return self_t(emplace<0>(rootid, x, s));
    }

    self_t emplace_replace(T x, U s) const {
        return self_t(emplace<1>(rootid, x, s));
    }

    self_t erase(T x) const {
        return self_t(erase(rootid, x));
    }

    std::pair<T, U> min() const {
        assert(!empty());
        int v = rootid;
        while (V[v].get_lch() != nullid) v = V[v].get_lch();
        return {V[v].k, V[v].s};
    }

    std::pair<T, U> max() const {
        assert(!empty());
        int v = rootid;
        while (V[v].get_rch() != nullid) v = V[v].get_rch();
        return {V[v].k, V[v].s};
    }

    // 0 <= k < size
   std::pair<T, U> kth_smallest(int k) const {
        assert(0 <= k && k < size());
        int v = rootid;
        while (true) {
            int szl = size(V[v].get_lch());
            if (szl == k) return {V[v].k, V[v].s};
            if (k < szl) {
                v = V[v].get_lch();
            } else {
                k -= szl + 1;
                v = V[v].get_rch();
            }
        }
    }

    // 0 <= k < size
    std::pair<T, U> kth_largest(int k) const {
        return kth_smallest(size() - 1 - k);
    }

    // x未満の要素数
    int ord(T x) const {
        int v = rootid, res = 0;
        while (v != nullid) {
            int szl = size(V[v].get_lch());
            if (x == V[v].k) return res + szl;
            if (x < V[v].k) {
                v = V[v].get_lch();
            } else {
                res += szl + 1;
                v = V[v].get_rch();
            }
        }
        return res;
    }
};
template<typename T, typename U>
std::vector<typename persistent_map<T, U>::node> persistent_map<T, U>::V;
template<typename T, typename U>
int persistent_map<T, U>::erased_node;
#endif