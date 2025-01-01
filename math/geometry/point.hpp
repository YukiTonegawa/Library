#ifndef _POINT_H_
#define _POINT_H_

template<typename T, typename T2>
struct Point{
    T x, y;
    bool operator == (const Point &p) const { return x == p.x && y == p.y; }
    bool operator != (const Point &p) const { return x != p.x || y != p.y; }
    bool operator < (const Point &p) const { return x < p.x || (x == p.x && y < p.y); }
    bool operator <= (const Point &p) const { return x < p.x || (x == p.x && y <= p.y); }
    // 0だと一列
    // 0未満だと反時計回り
    // 0より大きいと時計回り
    static T2 cross(const Point &a, const Point &b, const Point &c){
        return (T2)(b.y - a.y) * (c.x - a.x) - (T2)(c.y - a.y) * (b.x - a.x);
    }
};

#endif