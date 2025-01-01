#ifndef _SQRT_MOD_H_
#define _SQRT_MOD_H_
#include <random>
#include "../base/safe_mod.hpp"

// {x^2 ≡ aとなるxが存在するか, x}
// 素数mod
template<typename mint>
std::pair<bool, mint> sqrt_mod(mint a) {
    static std::random_device seed_gen;
    static std::mt19937_64 engine(seed_gen());

	if (a == 0) return {true, 0};
	if (mint::mod() == 2) return {true, 1};
    
    // オイラーの規準
    if (a.pow((mint::mod() - 1) / 2) != 1) {
        return {false, 0};
    }
    
	if (mint::mod() % 4 == 3) {
        return {true, a.pow(mint::mod() / 4 + 1)};
    }

	long long q = mint::mod() - 1, m = 0;
	while (q % 2 == 0) q >>= 1, m++;
	
	mint z;
    while (true) {
       z = engine();
       if (z.pow((mint::mod() - 1) / 2) == -1) break;
	}
	
    mint c = z.pow(q);
	mint t = a.pow(q);
	mint r = a.pow((q + 1) / 2);
	
    for(; m > 1; m--) {
        if (t.pow(1LL << (m - 2)) != 1) {
            r *= c;
            t *= c * c;
        }
		c *= c;
	}
	return {true, r};
}
#endif