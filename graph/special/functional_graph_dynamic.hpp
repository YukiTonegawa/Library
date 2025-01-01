#ifndef _DYNAMIC_FUNCTIONAL_GRAPH_H_
#define _DYNAMIC_FUNCTIONAL_GRAPH_H_
#include <vector>
#include <cassert>
#include <queue>
#include <algorithm>
#include "../tree/lct_path_prod.hpp"

template<typename S, S (*op)(S, S), S (*e)(), S (*_flip)(S)>
struct functional_graph_dynamic {
  private:
    using lct = lct_path_prod<S, op, e, _flip>;
    using node = typename lct::node;
    std::vector<node*> V;
    std::unordered_map<node*, int> idx;
    std::vector<int> cut_edge;

    // min, maxや+のときはO(1)にできる
    static S pow_monoid(S x, uint64_t k) {
        S res = e();
        while (k) {
            if (k & 1) res = op(res, x);
            x = op(x, x);
            k >>= 1;
        }
        return res;
    }

    void build_directed(const std::vector<int> &next, const std::vector<S> &val) {
        int N = next.size();
        V.resize(N);
        cut_edge.resize(N, -1);
        for (int i = 0; i < N; i++) {
            V[i] = lct::make_node(val[i]);
            idx[V[i]] = i;
        }
        std::vector<int> roots;
        for (int i = 0; i < N; i++) {
            int j = next[i];
            if (!lct::is_same(V[i], V[j])) {
                lct::_link(V[i], V[j]);
            } else {
                cut_edge[i] = j;
                roots.push_back(i);
            }
        }
        for (int r : roots) lct::evert(V[r]);
    }

    void build_undirected(const std::vector<std::pair<int, int>> &E, const std::vector<S> &val) {
        int N = E.size();
        V.resize(N);
        cut_edge.resize(N, -1);
        for (int i = 0; i < N; i++) {
            V[i] = lct::make_node(val[i]);
            idx[V[i]] = i;
        }
        for (auto [a, b] : E) {
            if (!lct::is_same(V[a], V[b])) {
                lct::_link(V[a], V[b]);
            }else{
                int r = to_idx(lct::get_root(V[a]));
                assert(cut_edge[r] == -1); // functional graphの要件を満たしていない
                cut_edge[a] = b;
                lct::evert(V[a]);
            }
        }
    }

    int to_idx(node *v) {
        auto itr = idx.find(v);
        assert(itr != idx.end());
        return itr->second;
    }

  public:
    functional_graph_dynamic() {}
    // 有向
    functional_graph_dynamic(const std::vector<int> &next, const std::vector<S> &val) { build(next, val); }
    // 無向
    functional_graph_dynamic(const std::vector<std::pair<int, int>> &E, const std::vector<S> &val) { build_undirected(E, val); }

    // 同じ連結成分にあるか
    bool same(int i, int j) {
        return lct::is_same(V[i], V[j]);
    }
    
    // iから辿り着けるループの周期
    int len_loop(int i){
        int root = to_idx(lct::get_root(V[i]));
        return lct::depth(V[cut_edge[root]]) + 1;
    }

    // iからループまでの距離
    int to_loop(int i) {
        int root = to_idx(lct::get_root(V[i]));
        node *l = lct::_lca(V[cut_edge[root]], V[i]);
        return lct::depth(V[i]) - lct::depth(l);
    }

    // iの値をxにする
    void set(int i, S x) {
        lct::set(V[i], x);
    }

    // iの値
    S get(int i) {
        return lct::get(V[i]);
    }

    // iからk回遷移した状態
    int move(int i, uint64_t k) {
        if (!k) return i;
        int d = lct::depth(V[i]);
        if (d >= k) return to_idx(lct::la(V[i], k));
        k -= d;
        int root = to_idx(lct::get_root(V[i]));
        node *c = V[cut_edge[root]];
        k %= (lct::depth(c) + 1);
        return (k == 0 ? root : to_idx(lct::la(c, k - 1)));
    }

    // op(i, f(i), f^2(i), ... f^k-1(i))
    S prod(int i, uint64_t k) {
        if (!k) return e();
        k--;
        if (!k) return lct::get(V[i]);
        int d = lct::depth(V[i]);
        node *r = lct::get_root(V[i]);
        if (d >= k) {
            node *v = lct::la(V[i], k);
            lct::evert(V[i]);
            S res = lct::path_prod(v);
            lct::evert(r);
            return res;
        }
        k -= d;
        S res = lct::path_prod(V[i]);
        int root = to_idx(r);
        node *c = V[cut_edge[root]];
        int len = lct::depth(c) + 1;
        if (k >= len){
            res = op(pow_monoid(lct::path_prod(c), k / len), res);
            k %= len;
        }
        if (!k) return _flip(res);
        node *v = lct::la(c, k - 1);
        lct::evert(v);
        res = op(lct::path_prod(c), res);
        lct::evert(r);
        return _flip(res);
    }
    
    // g(op(i, f(i), f^2(i), ... f^k-1(i))) = trueである最大のk (k <= 10^18程度であることが保証される場合)
    template<typename G>
    uint64_t max_right(int i, G g) {
        assert(g(e()));
        S sum = _flip(lct::path_prod(V[i]));
        node *r = lct::get_root(V[i]);
        int root = to_idx(r);
        if (!g(sum)) {
            lct::evert(V[i]);
            node *u = lct::max_right(r, g);
            lct::evert(r);
            if (!u) return 0;
            return lct::depth(V[i]) - lct::depth(u) + 1;
        }
        uint64_t K = lct::depth(V[i]) + 1;
        node *c = V[cut_edge[root]];
        S cycle_prod = _flip(lct::path_prod(c));
        uint64_t len = lct::depth(c) + 1;
        static std::array<S, 60> X;
        X[0] = cycle_prod;
        for (int j = 0; j < (int)X.size(); j++) {
            if (j > 0) X[j] = op(X[j - 1], X[j - 1]);
            if (g(op(sum, X[j]))) {
                sum = op(sum, X[j]);
                K += len * (1ULL << j);
            } else {
                while (--j >= 0) {
                    if (g(op(sum, X[j]))) {
                        sum = op(sum, X[j]);
                        K += len * (1ULL << j);
                    }
                }
                break;
            }
        }
        lct::evert(c);
        node *u = lct::max_right(r, [g, sum](auto x) { return g(op(sum, x)); });
        lct::evert(r);
        if (!u) return K;
        return K + (lct::depth(c) - lct::depth(u) + 1);
    }

    // i->jの最短距離 (異なる連結成分なら-1)
    int min_dist_undirected(int i, int j) {
        int d1 = to_loop(i), d2 = to_loop(j), res = 0;
        if (d1 < d2) {
            std::swap(i, j);
            std::swap(d1, d2);
        }
        i = move(i, d1 - d2);    
        res += d1 - d2;
        d1 = d2;
        if (i == j) return res;
        res += 2 * d1;
        int li = len_loop(i), diff = lct::depth(V[i]) - lct::depth(V[j]);
        if (diff < 0) diff += li;
        if (move(i, diff) != j) return -1; // 異なる連結成分
        return res + std::min(diff, li - diff);
    }

    // i->jの最短距離 (到達不可能な場合-1)
    int min_dist_directed(int i, int j) {
        int d1 = to_loop(i), d2 = to_loop(j);
        if (d2 > d1) return -1;
        i = move(i, d1 - d2);
        if (i == j) return d1 - d2;
        else if(d2) return -1; // i, jが違う枝
        int diff = lct::depth(V[i]) - lct::depth(V[j]);
        if (diff < 0) diff += len_loop(i);
        if (move(i, diff) != j) return -1; // 異なる連結成分
        return d1 + diff;
    }

    // 無向functional_graphで辺(i, j)を消して辺(a, b)を追加する
    // この操作を行った後で, functional_graphとして壊れている場合壊れる(消す辺がない場合, 木が発生する場合など)
    // 自己辺, 多重辺があってもいい
    void change_edge_undirected(int i, int j, int a, int b) {
        node *r = lct::get_root(V[i]);
        int root = to_idx(r);
        if (move(i, 1) != j) std::swap(j, i);
        assert(move(i, 1) == j);
        if (root != i) {
            node *c = V[cut_edge[root]];
            lct::cut_from_parent(V[i]);
            if (!lct::is_same(r, c)) {
                lct::_link(r, c);
                cut_edge[root] = -1;
            } else {
                lct::evert(r);
            }
        } else {
            cut_edge[root] = -1;
        }

        if (lct::is_same(V[a], V[b])) {
            lct::evert(V[a]);
            cut_edge[a] = b;
        } else {
            // 同じ連結成分に閉路が2つできると壊れる
            int ra = to_idx(lct::get_root(V[a]));
            int rb = to_idx(lct::get_root(V[b]));
            // 片方のみ閉路を持つ
            assert((cut_edge[ra] == -1) ^ (cut_edge[rb] != -1));
            if (cut_edge[ra] == -1) std::swap(ra, rb);
            lct::_link(V[ra], V[rb]);
            lct::evert(V[ra]);
        }
    }
    
    // 有向functional_graphでiの遷移先をjに変更する
    // 自己辺があってもいい
    void change_edge_directed(int i, int j) {
        node *r = lct::get_root(V[i]);
        int root = to_idx(r);
        int ch = cut_edge[root];
        if (root != i) {
            node *c = V[ch];
            lct::cut_from_parent(V[i]);
            if (!lct::is_same(r, c)) {
                lct::_link(r, c);
                cut_edge[root] = -1;
            } else {
                lct::evert(r);
            }
        } else {
            cut_edge[root] = -1;
        }
        if (lct::is_same(V[i], V[j])) {
            lct::evert(V[i]);
            cut_edge[i] = j;
        } else {
            r = lct::get_root(V[j]);
            lct::_link(V[i], V[j]);
            lct::evert(r);
        }
    }
};
#endif