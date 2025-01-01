#ifndef _BIT_CEIL_H_
#define _BIT_CEIL_H_

constexpr unsigned int bit_ceil(unsigned int n) {
    unsigned int x = 1;
    while (x < (unsigned int)(n)) x *= 2;
    return x;
}

constexpr int bit_ceil_log(unsigned int n) {
    int x = 0;
    while ((1 << x) < (unsigned int)(n)) x++;
    return x;
}
#endif