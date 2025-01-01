#ifndef _SYNCHRONOUS_MAXSUM_H_
#define _SYNCHRONOUS_MAXSUM_H_
#include <numeric>
#include <algorithm>
#include <array>

// 列x, yの区間chmin, add
// 区間 x + yの最大値
template<typename T>
struct synchronous_maxsum {
    static constexpr T inf = std::numeric_limits<T>::max();
    static constexpr T minf = std::numeric_limits<T>::min();

    struct S {
        T maxx, maxy;
        T smaxx, smaxy;
        std::array<T, 4> maxz;
        S() : maxx(minf), maxy(minf), smaxx(minf), smaxy(minf) { maxz.fill(minf); }
    };

    struct F {
        T addx, addy, upperx, uppery;
        F() : addx(0), addy(0), upperx(inf), uppery(inf) {}
    };
    
    static constexpr void op(S &v, const S &l, const S &r) {
        v.maxz.fill(minf);
        v.maxz[0] = std::max(l.maxz[0], r.maxz[0]);

        if (l.maxx == r.maxx) {
            v.maxx = l.maxx;
            v.smaxx = std::max(l.smaxx, r.smaxx);
            v.maxz[1] = std::max(l.maxz[1], r.maxz[1]);
        } else if (l.maxx > r.maxx) {
            v.maxx = l.maxx;
            v.smaxx = std::max(l.smaxx, r.maxx);
            v.maxz[1] = l.maxz[1];
            v.maxz[0] = std::max(v.maxz[0], r.maxz[1]);
        } else {
            v.maxx = r.maxx;
            v.smaxx = std::max(l.maxx, r.smaxx);
            v.maxz[0] = std::max(v.maxz[0], l.maxz[1]);
            v.maxz[1] = r.maxz[1];
        }
        if (l.maxy == r.maxy) {
            v.maxy = l.maxy;
            v.smaxy = std::max(l.smaxy, r.smaxy);
            v.maxz[2] = std::max(l.maxz[2], r.maxz[2]);
        } else if (l.maxy > r.maxy) {
            v.maxy = l.maxy;
            v.smaxy = std::max(l.smaxy, r.maxy);
            v.maxz[2] = l.maxz[2];
            v.maxz[0] = std::max(v.maxz[0], r.maxz[2]);
        } else {
            v.maxy = r.maxy;
            v.smaxy = std::max(l.maxy, r.smaxy);
            v.maxz[0] = std::max(v.maxz[0], l.maxz[2]);
            v.maxz[2] = r.maxz[2];
        }
        int flag = (l.maxx == v.maxx) + ((l.maxy == v.maxy) << 1);
        v.maxz[flag] = std::max(v.maxz[flag], l.maxz[3]);
        flag = (r.maxx == v.maxx) + ((r.maxy == v.maxy) << 1);
        v.maxz[flag] = std::max(v.maxz[flag], r.maxz[3]);
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
            if (s.maxz[i] != minf) {
                s.maxz[i] += add;
            }
        }
        if (f.upperx < s.maxx) {
            if (s.maxz[1] != minf) s.maxz[1] -= (s.maxx - f.upperx);
            if (s.maxz[3] != minf) s.maxz[3] -= (s.maxx - f.upperx);
            s.maxx = f.upperx;
        }
        if (f.uppery < s.maxy) {
            if (s.minz[2] != minf) s.minz[2] -= (s.maxy - f.uppery);
            if (s.minz[3] != minf) s.minz[3] -= (s.maxy - f.uppery);
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