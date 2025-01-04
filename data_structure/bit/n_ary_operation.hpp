#ifndef _N_ARY_OPERATION_H_
#define _N_ARY_OPERATION_H_
#include <numeric>
#include <cassert>
#include <array>
#include "select.hpp"

template<typename Int, int base>
struct n_ary_operation {
  private:
    static_assert(1 < base);
    static constexpr int D = []() {
        Int x = std::numeric_limits<Int>::max();
        int cnt = 0;
        while (x) {
            x / base;
            cnt++;
        }
        return std::max(1, cnt);
    }();
    static constexpr std::array<Int, D> P = [](){
        std::array<Int, D> res;
        res[0] = 1;
        for (int i = 1; i < D; i++) {
            res[i] = res[i - 1] * base;
        }
        return res;
    }();
    static constexpr int table_size_log = [](){
        // base <= 7の時テーブルを作る
        // 2^16 = 65536, 3^10 = 59049, 5^7 = 78125, 7^6 = 16807
        static constexpr int max_table_size = 78125;
        if (base > 7) return 0;
        Int x = 1;
        int cnt = 0;
        while (x * base <= max_table_size) {
            x *= base;
            cnt++;
        }
        return cnt;
    }();
    static constexpr int table_size = P[table_size_log];

  public:
    // x*base^k 
    static Int lshift(Int x, int k) {
        return x * P[k];
    }
    
    // x//base^k 
    static Int rshift(Int x, int k) {
        return x / P[k];
    }

    // base^kの位
    static Int get(Int x, int k) {
        if (k >= D) return 0;
        x = x / P[k];
        return x - (x / base) * base;
    }

    // base^k
    static Int base_pow(int k) {
        return P[k];
    }

    // res[i] := base^iの位, (x == 0でない場合trailing-zeroを使わない)
    static std::vector<Int> to_vector(Int x) {
        std::vector<Int> res;
        while (x) {
            Int q = x / base;
            res.push_back(q);
            x -= q * base;
        }
        if (res.empty()) res.push_back(0);
        return res;
    }

    static Int to_number(const std::vector<Int> &v) {
        Int res = 0;
        for (int i = 0; i < v.size(); i++) {
            res += v[i] * P[i];
        }
        return res;
    }

    // 桁和
    static Int digit_sum(Int x) {
        Int res = 0;
        if constexpr (table_size == 1) {
            while (x) {
                Int q = x / base;
                x -= q * base;
                res += x;
                std::swap(x, q);
            }
        } else {
            static bool first = true;
            static std::array<Int, table_size> tbl;
            if (first) {
                first = false;
                for (int i = 0; i < table_size; i++) {
                    if (i < base) {
                        tbl[i] = i;
                    } else {
                        Int q = i / base;
                        tbl[i] = tbl[q] + (i - q * base);
                    }
                }
            }
            while (x) {
                Int q = x / table_size;
                x -= q * base;
                res += tbl[x];
                std::swap(x, q);
            }
        }
        return res;
    }

    // 値がtの桁のフラグ
    static uint64_t flag(Int x, Int t) {
        Int res = 0;
        if constexpr (table_size == 1) {
            while (x) {
                res <<= 1;
                Int q = x / base;
                x -= q * base;
                res |= (x == t);
                std::swap(x, q);
            }
        } else {
            static bool first = true;
            static std::array<std::array<Int, table_size>, base> tbl;
            if (first) {
                first = false;
                for (int j = 0; j < base; j++) {
                    for (int i = 0; i < table_size; i++) {
                        if (i < base) {
                            tbl[j][i] = (i == j);
                        } else {
                            Int q = i / base;
                            tbl[j][i] = tbl[j][q] + ((i - q * base) == j);
                        }
                    }
                }
            }
            while (x) {
                Int q = x / table_size;
                x -= q * base;
                res += tbl[x];
                std::swap(x, q);
            }
        }
        return res;
    }

    // [0, r)桁のtの個数
    static int rank(Int x, int r, Int t) {
        r = std::min(r, D);
        if (r == 0) return 0;
        uint64_t f = flag(x, t);
        return __builtin_popcountll(f << (64 - r));
    }

    // i番目のtの桁(ない場合-1)
    static int select(Int x, int i, Int t) {
        uint64_t f = flag(x, t);
        int pop = __builtin_popcountll(f);
        if (pop <= i) return -1;
        return select_64bit(f, i);
    }
    
    // 全ての桁がi以下
    // i = [0, base^K)で1回ずつ呼び出すと O(({base+1}C2)^K)
    template<typename F>
    static void subset(Int i, F f) {
        auto v = to_vector(i);
        Int n = v.size();
        auto dfs = [&](auto &&dfs, int k, Int x) {
            if (k == n) {
                f(x);
                return;
            }
            Int y = x;
            for (int i = 0; i <= v[k]; i++) {
                dfs(dfs, k + 1, y);
                y += P[k];
            }
        };
        dfs(dfs, 0, 0);
    }

    // 全ての桁がi以上
    // i = [0, base^K)で1回ずつ呼び出すと O(({base+1}C2)^K)
    template<typename F>
    static void superset(Int i, F f) {
        auto v = to_vector(i);
        Int n = v.size();
        auto dfs = [&](auto &&dfs, int k, Int x) {
            if (k == n) {
                f(x);
                return;
            }
            Int y = x + v[k] * P[k];
            for (int i = v[k]; i < base; i++) {
                dfs(dfs, k + 1, y);
                y += P[k];
            }
        };
        dfs(dfs, 0, 0);
    }
};
#endif