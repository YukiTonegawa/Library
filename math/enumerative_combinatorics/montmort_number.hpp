#ifndef _MONTMORT_NUMBER_H_
#define _MONTMORT_NUMBER_H_
#include <vector>

// 0, 1, 2, 9, 44, 265, 1854, 14833
// 全てのiでP[i] != iである長さnの順列の数
// res[i] := ai (res[0]は未定義)
template<typename mint>
std::vector<mint> montmort_number_many(int n) {
    std::vector<mint> res{0, 1, 2};
    for (int i = 3; i <= n; i++) {
        res.push_back((i - 1) * (res[i - 2] + res[i - 1]));
    }
    return res;
}

template<typename mint>
mint montmort_number_many(int n) {
    return montmort_number_many<mint>(n)[n];
}
#endif