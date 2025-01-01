//#pragma GCC optimize ("O3")
//#pragma GCC target ("avx2, avx512")
/*
#include ".lib/template.hpp"
#include ".lib/data_structure/mo/mo_algorithm.hpp"

#include <immintrin.h>
#include <array>

// [0, 8^D)
// D = 6 -> 8^D = 262144
// D = 7 -> 8^D = 2097152
template<int D>
struct simd_set {
    static constexpr int K = 8;
    static constexpr int Klog = 3;
    static constexpr int N = 1 << (Klog * D);
    static constexpr int M = (N - 1) / (K - 1); // 1 + 8 + 8^2...8^(D-1)
    using i256 = __m256i;
    static std::array<i256, K> one;
    static std::array<i256, K> mask;
    std::array<i256, M> val;
    simd_set() {}

    // kが存在しないことが保証されている
    void on(int k) {
        k += M;
        while (k--) {
            int dir = k % K;
            k /= K;
            val[k] = _mm256_add_epi32(val[k], one[dir]);
        }
    }

    // kが存在することが保証されている
    void off(int k) {
        k += M;
        while (k--) {
            int dir = k % K;
            k /= K;
            val[k] = _mm256_sub_epi32(val[k], one[dir]);
        }
    }
    
    // [0, r)の要素数
    i256 rank(int r) {
        i256 sum = _mm256_set1_epi64x(0);
        r += M;
        while (r--) {
            int dir = r % K;
            r /= K;
            sum = _mm256_add_epi32(sum, _mm256_and_si256(val[r], mask[dir]));
        }
        return sum;
    }

    // [0, r)の要素数を返しつつrを追加
    // rが存在しないことが保証されている
    i256 on_rank(int r) {
        i256 sum = _mm256_set1_epi64x(0);
        r += M;
        while (r--) {
            int dir = r % K;
            r /= K;
            sum = _mm256_add_epi32(sum, _mm256_and_si256(val[r], mask[dir]));
            val[r] = _mm256_add_epi32(val[r], one[dir]);
        }
        return sum;
    }

    // [0, r)の要素数を返しつつrを削除
    // rが存在することが保証されている
    i256 off_rank(int r) {
        i256 sum = _mm256_set1_epi64x(0);
        r += M;
        while (r--) {
            int dir = r % K;
            r /= K;
            sum = _mm256_add_epi32(sum, _mm256_and_si256(val[r], mask[dir]));
            val[r] = _mm256_sub_epi32(val[r], one[dir]);
        }
        return sum;
    }

    long long sum32(__m256i x) {
        return (long long)_mm256_extract_epi32(x, 0) + _mm256_extract_epi32(x, 1) + _mm256_extract_epi32(x, 2) + _mm256_extract_epi32(x, 3) +
                      _mm256_extract_epi32(x, 4) + _mm256_extract_epi32(x, 5) + _mm256_extract_epi32(x, 6) + _mm256_extract_epi32(x, 7);
    }
};
template<int D>
std::array<typename simd_set<D>::i256, simd_set<D>::K> init_one() {
    std::array<typename simd_set<D>::i256, simd_set<D>::K> res;
    res.fill(_mm256_set1_epi64x(0));
    res[0] = _mm256_insert_epi32(res[0], 1, 0);
    res[1] = _mm256_insert_epi32(res[1], 1, 1);
    res[2] = _mm256_insert_epi32(res[2], 1, 2);
    res[3] = _mm256_insert_epi32(res[3], 1, 3);
    res[4] = _mm256_insert_epi32(res[4], 1, 4);
    res[5] = _mm256_insert_epi32(res[5], 1, 5);
    res[6] = _mm256_insert_epi32(res[6], 1, 6);
    res[7] = _mm256_insert_epi32(res[7], 1, 7);
    return res;
}
template<int D>
std::array<typename simd_set<D>::i256, simd_set<D>::K> init_mask() {
    std::array<typename simd_set<D>::i256, simd_set<D>::K> res;
    res[0] = _mm256_set1_epi64x(0);
    res[1] = _mm256_insert_epi32(res[0], ~0, 0);
    res[2] = _mm256_insert_epi32(res[1], ~0, 1);
    res[3] = _mm256_insert_epi32(res[2], ~0, 2);
    res[4] = _mm256_insert_epi32(res[3], ~0, 3);
    res[5] = _mm256_insert_epi32(res[4], ~0, 4);
    res[6] = _mm256_insert_epi32(res[5], ~0, 5);
    res[7] = _mm256_insert_epi32(res[6], ~0, 6);
    return res;
}
template<int D>
std::array<typename simd_set<D>::i256, simd_set<D>::K> simd_set<D>::one = init_one<D>();
template<int D>
std::array<typename simd_set<D>::i256, simd_set<D>::K> simd_set<D>::mask = init_mask<D>();



template<typename T>
std::vector<long long> offline_static_range_inversion(const std::vector<T> &V, const std::vector<std::pair<int, int>> &Q) {
    struct range_inversion_st {
        simd_set<6> b;
        std::vector<int> a;
        int n, all = 0;
        __m256i sum = _mm256_set1_epi64x(0);
        ll allsum = 0;
        range_inversion_st(int n): n(n) {}
        void add_left(int i) {
            all++;
            sum = _mm256_add_epi32(sum, b.on_rank(a[i]));
        }
        void del_left(int i) {
            all--;
            sum = _mm256_sub_epi32(sum, b.off_rank(a[i]));
        }
        void add_right(int i) {
            allsum += all++;
            sum = _mm256_sub_epi32(sum, b.on_rank(a[i]));
        }
        void del_right(int i) {
            allsum -= --all;
            sum = _mm256_add_epi32(sum, b.off_rank(a[i]));
        }
    };
    int n = V.size(), q = Q.size();
    range_inversion_st ri(n);
    std::vector<int> tmp(n);
    std::vector<std::pair<T, int>> z;
    for (int i = 0; i < n; i++) z.push_back({V[i], i});
    std::sort(z.begin(), z.end());
    for (int i = 0; i < n; i++) tmp[z[i].second] = i;
    ri.a = tmp;
    ri.b = simd_set<6>();
    mo_algorithm mo(n, ri);
    for (auto [l, r] : Q) mo.add_query(l, r);
    std::vector<long long> ans(q);
    while(true) { 
        int i = mo.process();
        if (i == -1) break;
        ans[i] = ri.allsum + ri.b.sum32(ri.sum);
    }
    return ans;
}

int main() {
    io_init();
    int n, q;
    std::cin >> n >> q;
    vector<int> tmp(n);
    vector<pair<int, int>> Q(q);
    range(i, 0, n) std::cin >> tmp[i];
    range(i, 0, q) std::cin >> Q[i].first>> Q[i].second;
    for (long long x : offline_static_range_inversion(tmp, Q)) {
        std::cout << x << '\n';
    }
}
*/
