#ifndef _PROJECT_SELECTION_MULTI_H_
#define _PROJECT_SELECTION_MULTI_H_
#include "dinic.hpp"

template<typename T, T inf>
struct project_selection_multi_option {
  private:
    int N, s, t;
    T add;
    std::vector<int> begin;
    dinic<T> G;
    // aがfであることを表す辺
    std::pair<int, int> get_edge(int a, int f) {
        int num = num_state(a);
        assert(0 <= f && f < num);
        int l = (f == 0 ? s : begin[a] + f - 1);
        int r = (f + 1 == num ? t : begin[a] + f);
        return {l, r};
    }

  public:
    project_selection_multi_option() : N(0) {}
    project_selection_multi_option(int N, int K) : project_selection_multi_option(std::vector<int>(N, K)) {}
    project_selection_multi_option(const std::vector<int> &_S) : N(_S.size()), add(0), begin(_S) {
        begin.insert(begin.begin(), 0);
        for (int i = 1; i < begin.size(); i++) {
            assert(begin[i] > 0);
            begin[i] = begin[i] - 1 + begin[i - 1];
        }
        s = begin.back();
        t = s + 1;
        G = dinic<T>(t + 1);
        for (int i = 0; i < N; i++) {
            int num = num_state(i);
            for (int j = 0; j < num - 2; j++) {
                G.add_edge(begin[i] + j + 1, begin[i] + j, inf);
            }
        }
    }

    // aの可能な状態
    int num_state(int a) const {
        return begin[a + 1] - begin[a] + 1;
    }

    // aがfなら+x(任意)
    void condition_1_variable(int a, int f, T x) {
        assert(0 <= a && a < N);
        int num = num_state(a);
        assert(0 <= f && f < num);
        if (x == 0) return;
        if (x > 0) {
            add += x;
            for (int i = 0; i < num; i++) {
                if (i == f) continue;
                auto [l, r] = get_edge(a, i);
                G.add_edge(l, r, x);
            }
        } else {
            auto [l, r] = get_edge(a, f);
            G.add_edge(l, r, -x);
        }
    }
    
    // x[i] := aがiなら+x[i](任意) 
    void condition_1_variable(int a, const std::vector<T> &x) {
        assert(0 <= a && a < N);
        int num = num_state(a);
        assert(x.size() == num);
        T mx = *max_element(x.begin(), x.end());
        add += mx;
        for (int i = 0; i < num; i++) {
            auto [l, r] = get_edge(a, i);
            G.add_edge(l, r, mx - x[i]);
        }
    }

    // {ai, li}が与えられる. 条件ai >= liを1つでも満たすなら+x(<0)
    void condition_greater_either(std::vector<std::pair<int, int>> C, T x) {
        aassert(x <= 0);
        if (x == 0) return;
        for (auto &p : C) {
            p.second = std::max(p.second, 0);
            if (p.second == 0) {
                add += x;
                return;
            }
        }
        int w = G.add_vertex();
        G.add_edge(w, t, -x);
        for (auto &p : C) {
            int a = p.first;
            if (p.second >= num_state(a)) continue;
            G.add_edge(begin[a] + p.second - 1, w, inf);
        }
    }

    // {ai, ri}が与えられる. 条件ai < riを1つでも満たすなら+x(<0)
    void condition_less_either(std::vector<std::pair<int, int>> C, T x) {
        aassert(x <= 0);
        if (x == 0) return;
        for (auto &p : C) {
            int a = p.first;
            int numa = num_state(a);
            p.second = std::min(p.second, numa);
            if (p.second == numa) {
                add += x;
                return;
            }
        }
        int w = G.add_vertex();
        G.add_edge(s, w, -x);
        for (auto &p : C) {
            if (p.second == 0) continue;
            int a = p.first;
            G.add_edge(w, begin[a] + p.second - 1, w, inf);
        }
    }

    // aが[la, state(a)), bが[0, rb)だと+x(<0)
    void condition_less_greater(int a, int b, int la, int rb, T x) {
        int numa = num_state(a);
        int numb = num_state(b);
        assert(0 <= a && a < N);
        assert(0 <= b && b < N);
        assert(x <= 0);
        if (la >= numa || rb <= 0 || x == 0) return;
        la = std::max(la, 0);
        rb = std::min(rb, numb);
        int A = (la == 0 ? s : begin[a] + la - 1);
        int B = (rb == numb ? t : begin[b] + rb - 1);
        G.add_edge(A, B, -x);
    }

    // x[i][j] := aがiでbがjのときスコア+x[i][j] (任意)
    // -x(コスト行列)がmonge性を満たす必要がある
    // 
    void condition_2_variable(int a, int b, std::vector<std::vector<T>> x) {
        int numa = num_state(a);
        int numb = num_state(b);
        if (numa == 1) {
            condition_1_variable(b, x[0]);
            return;
        }
        if (numb == 1) {
            std::vector<T> A(numa);
            for (int i = 0; i < numa; i++) A[i] = x[i][0];
            condition_1_variable(a, A);
            return;
        }
        for (int i = 0; i < numa; i++) {
            for (int j = 0; j < numb; j++) {
                x[i][j] = -x[i][j];
            }
        }
        std::vector<T> A(numa, 0), B(numb, 0);
        std::vector<std::vector<T>> AB(numa, std::vector<T>(numb, 0));
        T allsub = 0;
        for (int i = 0; i < numa - 1; i++) {
            for (int j = 0; j < numb - 1; j++) {
                T x0 = x[i][j] - allsub;
                T x1 = x[i][j + 1] - allsub;
                T x2 = x[i + 1][j] - allsub;
                T x3 = x[i + 1][j + 1] - allsub;
                assert(x1 + x2 >= x0 + x3); // monge
                add -= x0;
                B[j + 1] -= x1 - x0;
                A[i + 1] -= x3 - x1;
                AB[i + 1][j] += x1 + x2 - x0 - x3;
                allsub += x0;
                for (int k = i; k < numa; k++) x[k][j + 1] -= x1 - x0;
                for (int k = j; k < numb; k++) x[i + 1][k] -= x3 - x1;
                x[i + 1][j] -= x1 + x2 - x0 - x3;
            }
        }
        condition_1_variable(a, A);
        condition_1_variable(b, B);
        for (int i = 1; i < numa; i++) {
            for (int j = 0; j < numb - 1; j++) {
                T diff = AB[i][j] - AB[i - 1][j];
                G.add_edge(begin[a] + i - 1, begin[b] + j, diff);
            }
        }
    }

    // {infをcutしないような解があるか, 最大スコア, 各変数の値}
    std::tuple<bool, T, std::vector<int>> max_score() {
        T f = G.max_flow(s, t, inf);
        if (f == inf) return {false, 0, {}};
        T score = add - f;
        auto S = G.min_cut(s);
        std::vector<int> res(N, 0);
        for (int i = 0; i < N; i++) {
            int l = begin[i], r = begin[i + 1];
            while (l + res[i] < r && S[l + res[i]]) res[i]++;
        }
        return {true, score, res};
    }
};
#endif