#ifndef _PSEUDO_TREE_H_
#define _PSEUDO_TREE_H_
#include <vector>
#include <cassert>
#include <algorithm>

// 0-indexedのk分木
// 頂点iから ki + 1, ki + 2....ki + kに辺が伸びている(nを超える場合はなし)
// (= 頂点iから (i - 1) / kに辺が伸びている(0からはなし))
template<typename I, int k>
struct k_ary_tree {
    static constexpr int bitlen = sizeof(I) * 8;
    I N, M;
    std::vector<I> Lelem; // 各深さの最左ノード
    std::vector<I> kpow;
    k_ary_tree() {}
    k_ary_tree(I n): N(n) {
        assert(n);
        M = 1;
        Lelem.push_back(0);
        while(M < N){
            Lelem.push_back(M);
            // Mは最大でNK程度になり, N, kが大きいとMがオーバーフローする可能性がある
            assert((std::numeric_limits<I>::max() - 1) / k >= M);
            M = (M * k + 1);
        }
        I p = 1;
        for(int i = 0; i < Lelem.size(); i++){
            kpow.push_back(p);
            p *= k;
        }
    }
    int height(){
        return Lelem.size();
    }
    // aの深さ
    int depth(I a){
        int ret = 0;
        while(a){
            a = (a - 1) / k;
            ret++;
        }
        return ret;
    }
    // {aの深さ, aと同じ深さのノードでaより小さいものの数}
    std::pair<int, I> index_sibling(I a){
        int d = depth(a);
        return {d, a - Lelem[d]};
    }
    // 深さが最も深いノードの数
    I num_deepest(){
        return N - Lelem.back();
    }
    // 葉の数
    I num_leaf(){
        I nd = num_deepest();
        I ALLLEAF = M - Lelem.back();
        return nd + (ALLLEAF - nd) / k;
    }
    // aの部分木に含まれる最も深いノードの数
    I num_subdeepest(I a){
        auto [d, si] = index_sibling(a);
        int hdiff = (int)Lelem.size() - d;
        // 完全k分木ならk ^ (h - 1 - d)個の葉がある
        return std::max(I(0), num_deepest() - si * kpow[hdiff - 1]);
    }
    // aの部分木に含まれる葉の数
    I num_subleaf(I a){
        auto [d, si] = index_sibling(a);
        int hdiff = (int)Lelem.size() - d;
        // 完全k分木ならk ^ (h - 1 - d)個の葉がある
        I subdeep = std::max(I(0), num_deepest() - si * kpow[hdiff - 1]);
        return subdeep + (kpow[hdiff - 1] - subdeep) / k;
    }
    // aの部分木のサイズ
    I num_subtree(I a){
        auto [d, si] = index_sibling(a);
        int hdiff = (int)Lelem.size() - d;
        // 完全k分木ならk ^ (h - 1 - d)個の葉がある
        I subdeep = std::max(I(0), num_deepest() - si * kpow[hdiff - 1]);
        return Lelem[hdiff - 1] + subdeep;
    }
    // aが葉か
    bool is_leaf(I a){
        return Lelem.back() <= a;
    }
    I dist(I a, I b){
        return dist2(a, b).second;
    }
    // a, bの{lca, 最短距離}
    std::pair<I, I> dist2(I a, I b){
        I d = 0;
        while(a != b){
            if(a < b) std::swap(a, b);
            a = (a - 1) / k;
            d++;
        }
        return {a, d};
    }
    // 親, ない場合は-1
    I parent(I a){
        return a ? (a - 1) / k : -1;
    }
  // aのt個親, 深さを超える場合は-1
    I la(I a, int t){
        for(int i = 0; i < t; i++){
            if(!a) return -1;
            a = (a - 1) / k;
        }
        return a;
    }
    // lca
    I lca(I a, I b){
        return dist2(a, b).first;
    }
    // {ノードの深さ, その部分木の深さh-1のノードがいくつ欠けているか}でノードを分類すると, その種類数は高々3h
    // {個数, ノードの深さ, 深さh-1のノードがいくつ欠けているか}を返す
    std::vector<std::tuple<I, I, I>> depth_frequency_decompose(){
        std::vector<std::tuple<I, I, I>> ret;
        I x = N - 1, nd = 1;
        int h = (int)Lelem.size();
        for(int d = h - 1; d >= 0; d--){
            I L = x - Lelem[d], R = kpow[d] - 1 - L;
            if(L) ret.push_back({L, d, 0});
            if(R) ret.push_back({R, d, kpow[h - 1 - d]});
            ret.push_back({1, d, kpow[h - 1 - d] - nd});
            if(d){
                x--;
                nd += kpow[h - 1 - d] * (x % k);
                x /= k;
            }
        }
        return ret;
    }
    // aの部分木の頻度テーブル(ans[i] := aの部分木に含まれaとの距離がiのノード数)
    std::vector<I> depth_frequency(I a){
        if(a >= N) return {};
        std::vector<I> ret;
        int t = 0;
        while(a < M){
            if(N <= a){
                ret.push_back(std::max(I(0), kpow[t++] - (a - N + 1)));
                return ret;
            }else{
                ret.push_back(kpow[t++]);
            }
            a = a * k + k;
        }
        return ret;
    }
    // aの部分木の頂点でaとの距離がdの頂点の数
    I count_dist_subtree(I a, int d){
        if(d < 0 || a >= N) return 0;
        int da = depth(a);
        return __count_dist_subtree(a, da, d);
    }
    I __count_dist_subtree(I a, int da, int d){
        if(d < 0 || a >= N) return 0;
        int h = Lelem.size();
        if(da + d >= h) return 0;
        if(da + d < h - 1){
            return kpow[d];
        }else{
            I ldeep = (a - Lelem[da]) * kpow[d];
            return std::min(kpow[d], std::max(I(0), num_deepest() - ldeep));
        }
    }
    // aとの距離がdの頂点の数
    I count_dist(I a, int d){
        if(d < 0 || a >= N) return 0;
        I ans = 0;
        int da = depth(a);
        while(d >= 0){
            ans += __count_dist_subtree(a, da, d);
            if(!a) return ans;
            ans -= __count_dist_subtree(a, da, d - 2);
            d--, da--;
            a = (a - 1) / k;
        }
        return ans;
    }
};

#endif