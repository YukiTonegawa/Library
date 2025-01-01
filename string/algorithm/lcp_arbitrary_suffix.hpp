#ifndef _LCP_ARBITRARY_SUFFIX_H_
#define _LCP_ARBITRARY_SUFFIX_H_
#include <vector>
#include "suffix_array.hpp"
#include "../../data_structure/range_query/sparse_table.hpp"

struct lcp_arbitrary_suffix {
    static constexpr int _min(int a, int b){return std::min(a, b);}
    std::vector<int> sa;
    std::vector<int> rank;
    rmq<int> st;

    template<typename T>
    lcp_arbitrary_suffix(const std::vector<T> &s): sa(suffix_array(s)), rank(rank_array(sa)), st(lcp_array(s, sa)) {}
    lcp_arbitrary_suffix(const std::string &s): sa(suffix_array(s)), rank(rank_array(sa)), st(lcp_array(s, sa)) {}
    lcp_arbitrary_suffix(const std::vector<int> &_sa, const std::vector<int> &_lcp, const std::vector<int> &_rank) : sa(_sa), rank(_rank), st(_lcp) {}

    // v[i...n)とv[j...n)のlcp
    int lcp(int i, int j) {
        if (i == j) return rank.size() - i;
        i = rank[i], j = rank[j];
        if (i > j) std::swap(i, j);
        return st.query(i, j);
    }
};

#endif