#ifndef _SEQUENCE_SUM_H_
#define _SEQUENCE_SUM_H_

// ak + b のl <= k < rにおける和
template<typename T = long long>
T arithmetic_sum(T a, T b, T l, T r) {
    return a * (r * (r - 1) - l * (l - 1)) / 2 + b * (r - l);
}
// a * k^2の　l <= k < rにおける和
template<typename T = long long>
T square_sum(T a, T l, T r) {
    return (r * (r - 1) * (2 * r - 1) - l * (l - 1) * (2 * l - 1)) / 6 * a;
}
// a^k * b のl <= k < rにおける和
// b * (a^r - a^l) / (a - 1)
template<typename T>
T geometric_sum(T a, T b, T l, T r) {
    if (a == 1) return 0;
    return b * (a.pow(r) - a.pow(l)) / (a - 1);
}
#endif