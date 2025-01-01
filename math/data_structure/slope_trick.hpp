#ifndef _SLOPE_TRICK_H_
#define _SLOPE_TRICK_H_
#include <queue>
#include <vector>
#include <limits>
#include <functional>

template<typename T>
struct slope_trick {
  private:
    static constexpr T xmin = std::numeric_limits<T>::min() / 2;
    static constexpr T xmax = std::numeric_limits<T>::max() / 2;
    T min_fx;
    std::priority_queue<T, std::vector<T>> L;
    std::priority_queue<T, std::vector<T>, std::greater<T>> R;
    T add_l, add_r;
    T max_l() {
        return L.empty() ? xmin : L.top() + add_l;
    }
    T min_r() {
        return R.empty() ? xmax : R.top() + add_r;
    }

  public:
    slope_trick() : min_fx(0), add_l(0), add_r(0) {}

    // 傾きが変わる点が単調な場合の線形時間構築
    // {a, b}
    // b = 0: add_left(a)
    // b = 1: add_right(a)
    // b = 2: add_abs(a)
    slope_trick(const std::vector<std::pair<T, int>> &v): min_fx(0), add_l(0), add_r(0) {
        std::vector<T> Ltmp;
        std::deque<T> Rtmp;
        auto addl = [&](T x) -> void {
            if(!Rtmp.empty()) {
                min_fx += x - Rtmp.front();
                Rtmp.push_back(x);
                x = Rtmp.front();
                Rtmp.pop_front();
            }
            Ltmp.push_back(x);
        };
        auto addr = [&](T x) -> void { Rtmp.push_back(x); };
        for (auto [a, b] : v) {
            if (b == 0) addl(a);
            else if (b == 1) addr(a);
            else addl(a), addr(a);
        }
        std::reverse(Ltmp.begin(), Ltmp.end());
        L = std::priority_queue<T, std::vector<T>>(Ltmp.begin(), Ltmp.end());
        R = std::priority_queue<T, std::vector<T>, std::greater<T>>(Rtmp.begin(), Rtmp.end());
    }

    // 最小値
    T min() {
        return min_fx;
    }

    // 最小値を取る区間[l, r]
    std::pair<T, T> min_range() {
        return std::make_pair(max_l(), min_r());
    }

    // 定数xを足す
    void add_const(T x) {
        min_fx += x;
    }

    // max(0, a - x)を足す
    void add_left(T a) {
        T r0 = min_r();
        if (a > r0){
            min_fx += a - r0;
            R.push(a - add_r);
            a = R.top() + add_r;
            R.pop();
        }
        L.push(a - add_l);
    }

    // max(0, x - a)を足す
    void add_right(T a) {
        T l0 = max_l();
        if (a < l0) {
            min_fx += l0 - a;
            L.push(a - add_l);
            a = L.top() + add_l;
            L.pop();
        }
        R.push(a - add_r);
    }
    
    // |x - a|を足す
    void add_abs(T a) {
        add_left(a);
        add_right(a);
    }

    // 左側をクリア [xlow, 左の傾きの終わり]をmin_fxにする
    void accumulate_min_left() {
        //L.clear();
        L = std::priority_queue<T, std::vector<T>>();
    }

    // 右側をクリア [右の傾きの始まり, xmax]をmin_fxにする
    void accumulate_min_right() {
        //R.clear();
        R = std::priority_queue<T, std::vector<T>, std::greater<T>>();
    }

    // 左側を右にa, 右側を右にb平行移動する
    // 負でもok
    // l0 + a > r0 + bだと左右で交差して壊れる
    void shift(T a, T b) {
        T l0 = max_l();
        T r0 = min_r();
        assert(r0 - l0 >= a - b);
        add_l += a;
        add_r += b;
    }
    
    // 傾きが変わる点で分割(区間[l, r], ax + bで表される線分の集合になる)
    // {l, a, b}
    std::vector<std::tuple<T, T, T>> to_func() {
        std::vector<std::tuple<T, T, T>> res;
        auto tmpl = L;
        T d = 0, y1 = min_fx;
        while (!tmpl.empty()) {
            T x0 = tmpl.top() + add_l;
            T x1 = res.empty() ? x0 : std::get<0>(res.back());
            T c = -d * x1 + y1;
            res.push_back({x0, d, c});
            y1 = x0 * d + c;
            while (!tmpl.empty() && tmpl.top() + add_l == x0) {
                tmpl.pop();
                d--;
            }
        }
        res.push_back({xmin, d, -d * (res.empty() ? 0 : std::get<0>(res.back())) + y1});
        std::reverse(res.begin(), res.end());
        auto tmpr = R;
        d = 0;
        T y0 = min_fx;
        while (!tmpr.empty()) {
            T x0 = tmpr.top() + add_r;
            while (!tmpr.empty() && tmpr.top() + add_r == x0) {
                tmpr.pop();
                d++;
            }
            res.push_back({x0, d, -d * x0 + y0});
            if (!tmpr.empty()) {
                T x1 = tmpr.top() + add_r;
                y0 = std::get<1>(res.back()) * x1 + std::get<2>(res.back());
            }
        }
        return res;
    }
};
#endif