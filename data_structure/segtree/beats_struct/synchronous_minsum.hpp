#ifndef _SYNCHRONOUS_MINSUM_H_
#define _SYNCHRONOUS_MINSUM_H_
#include <numeric>
#include <algorithm>
#include <array>
// 列x, yの区間chmin, add
// 区間 x + yの最小値
template<typename T>
struct synchronous_minsum {
    static constexpr T inf = std::numeric_limits<T>::max();
    static constexpr T minf = std::numeric_limits<T>::min();

    struct S {
        T maxx, maxy;
        T smaxx, smaxy;
        std::array<T, 4> minz;
        S() : maxx(minf), maxy(minf), smaxx(minf), smaxy(minf) { minz.fill(inf); }
    };

    struct F {
        T addx, addy, upperx, uppery;
        F() : addx(0), addy(0), upperx(inf), uppery(inf) {}
    };
    
    static constexpr void op(S &v, const S &l, const S &r) {
        v.minz.fill(inf);
        v.minz[0] = std::min(l.minz[0], r.minz[0]);
        if (l.maxx == r.maxx) {
            v.maxx = l.maxx;
            v.smaxx = std::max(l.smaxx, r.smaxx);
            v.minz[1] = std::min(l.minz[1], r.minz[1]);
        } else if (l.maxx > r.maxx) {
            v.maxx = l.maxx;
            v.smaxx = std::max(l.smaxx, r.maxx);
            v.minz[1] = l.minz[1];
            v.minz[0] = std::min(v.minz[0], r.minz[1]);
        } else {
            v.maxx = r.maxx;
            v.smaxx = std::max(l.maxx, r.smaxx);
            v.minz[0] = std::min(v.minz[0], l.minz[1]);
            v.minz[1] = r.minz[1];
        }
        if (l.maxy == r.maxy) {
            v.maxy = l.maxy;
            v.smaxy = std::max(l.smaxy, r.smaxy);
            v.minz[2] = std::min(l.minz[2], r.minz[2]);
        } else if (l.maxy > r.maxy) {
            v.maxy = l.maxy;
            v.smaxy = std::max(l.smaxy, r.maxy);
            v.minz[2] = l.minz[2];
            v.minz[0] = std::min(v.minz[0], r.minz[2]);
        } else {
            v.maxy = r.maxy;
            v.smaxy = std::max(l.maxy, r.smaxy);
            v.minz[0] = std::min(v.minz[0], l.minz[2]);
            v.minz[2] = r.minz[2];
        }
        int flag = (l.maxx == v.maxx) + ((l.maxy == v.maxy) << 1);
        v.minz[flag] = std::min(v.minz[flag], l.minz[3]);
        flag = (r.maxx == v.maxx) + ((r.maxy == v.maxy) << 1);
        v.minz[flag] = std::min(v.minz[flag], r.minz[3]);
    }

    static constexpr S e() {
        return S();
    }

    static constexpr void mapping(const F &f, S &s, int len) {
        T add = 0;
        if (f.addx) {
            add += f.addx;
            s.maxx += f.addx;
            if (s.smaxx != minf) s.smaxx += f.addx;
        }
        if (f.addy) {
            add += f.addy;
            s.maxy += f.addy;
            if (s.smaxy != minf) s.smaxy += f.addy;
        }
        for (int i = 0; i < 4; i++) {
            if (s.minz[i] != inf) {
                s.minz[i] += add;
            }
        }
        if (f.upperx < s.maxx) {
            if (s.minz[1] != inf) s.minz[1] -= (s.maxx - f.upperx);
            if (s.minz[3] != inf) s.minz[3] -= (s.maxx - f.upperx);
            s.maxx = f.upperx;
        }
        if (f.uppery < s.maxy) {
            if (s.minz[2] != inf) s.minz[2] -= (s.maxy - f.uppery);
            if (s.minz[3] != inf) s.minz[3] -= (s.maxy - f.uppery);
            s.maxy = f.uppery;
        }
    }

    static constexpr void composition(const F &f, F &a) {
        a.addx += f.aaddx;
        a.addy += f.addy;
        if (f.addx && a.upperx != inf) a.upperx += f.addx;
        if (f.addy && a.uppery != inf) a.uppery += f.addy;
        a.upperx = std::min(a.upperx, f.upperx);
        a.uppery = std::min(a.uppery, f.uppery);
    }

    static constexpr bool break_add(const S &a, const F &f) {
        return false;
    }

    static constexpr bool tag_add(const S &a, const F &f) {
        return true;
    }

    static constexpr bool break_chmin_x(const S &a, const F &f) {
        return a.maxx <= f.upperx;
    }

    static constexpr bool tag_chmin_x(const S &a, const F &f) {
        return a.smaxx < f.upperx;
    }

    static constexpr bool break_chmin_y(const S &a, const F &f) {
        return a.maxy <= f.uppery;
    }

    static constexpr bool tag_chmin_y(const S &a, const F &f) {
        return a.smaxy < f.uppery;
    }
    
    static constexpr F transform(const S &a, const F &f) {
        return f;
    }
};
#endif