#ifndef _MST_INCREMENTAL_H_
#define _MST_INCREMENTAL_H_
#include <cassert>
#include <numeric>
#include "../tree/lct_min_node.hpp"

template<typename W>
struct mst_incremental {
  private:
    static constexpr bool cmp(W a, W b) { return a > b; };
    static constexpr W e() { return std::numeric_limits<W>::min(); }
    using lct = lct_min_node<W, cmp, e>;
    using node = typename lct::node;
    W wsum;
    int used_edge;
    std::vector<node*> V;
    std::vector<node*> E;
    // i本目の辺を追加したことで消えた辺
    // コストが高い, 自己辺などの理由で追加されなかった場合その辺自身
    std::vector<node*> replaced;

  public:
    mst_incremental() : wsum(0), used_edge(0) {}

    // 次数0の頂点を追加して頂点番号を返す
    int add_vertex() {
        int id = V.size();
        V.push_back(lct::make_node(e()));
        V.back()->_val.a = id;
        return id;
    }

    // 辺を追加して辺番号を返す
    int add_edge(int a, int b, W c) {
        assert(0 <= a && a < V.size());
        assert(0 <= b && b < V.size());
        int id = E.size();
        auto ei = lct::make_node(c);
        ei->_val.a = a;
        ei->_val.b = b;
        E.push_back(ei);
        if (a == b) {
            replaced.push_back(ei);
            return id;
        }
        if (lct::is_same(V[a], V[b])) {
            node *maxe = lct::path_min_node(V[a], V[b]);
            if (maxe->_val.x <= c) {
                replaced.push_back(ei);
                return id;
            }
            replaced.push_back(maxe);
            wsum -= maxe->_val.x;
            int c = maxe->_val.a;
            int d = maxe->_val.b;
            lct::_cut(V[c], maxe);
            lct::_cut(V[d], maxe);
        } else {
            used_edge++;
            replaced.push_back(nullptr);
        }
        lct::_link(V[a], E.back());
        lct::_link(V[b], E.back());
        wsum += c;
        return id;
    }

    // 最後に追加した辺を消す
    // 辺を消して木構造が変化しなかった場合false
    bool erase_last_edge() {
        assert(!replaced.empty());
        node *v = replaced.back();
        node *u = E.back();
        replaced.pop_back();
        E.pop_back();
        if (v == u) return false;
        wsum -= u->_val.x;
        int a = u->_val.a;
        int b = u->_val.b;
        lct::_cut(V[a], u);
        lct::_cut(V[b], u);
        if (v) {
            wsum += v->_val.x;
            a = v->_val.a;
            b = v->_val.b;
            lct::_link(V[a], v);
            lct::_link(V[b], v);
        } else {
            used_edge--;
        }
        return true;
    }

    bool is_same(int a, int b) {
        assert(0 <= a && a < V.size());
        assert(0 <= a && a < V.size());
        return lct::is_same(V[a], V[b]);
    }

    bool is_mst() {
        int N = V.size();
        return N == 0 || used_edge == N - 1;
    }

    // 連結成分の数
    int num_components() {
        int N = V.size();
        return N - used_edge;
    }

    // 呼んだ時点の最小全域森の重みの和
    W weight_sum() {
        return wsum;
    }
};
#endif