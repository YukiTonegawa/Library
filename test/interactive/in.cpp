#include "../lib/template.hpp"

int main(){
    int n = random_number(0, 10) * 2 + 1;
    auto v = random_permutation(2 * n);
    range(i, 0, 2 * n) v[i] = (v[i] > n);
    //std::cerr << v << endl;

    std::cout << n << endl;
    // cerr
    while(true){
        char c;
        std::cin >> c;
        if(c == '?'){
            auto v2 = read_vec<int>(n);
            int cnt = 0;
            for(int i : v2){
                if(v[i - 1]){
                    cnt++;
                }else{
                    cnt--;
                }
            }
            std::cout << (cnt > 0 ? "Red" : "Blue") << endl;
        }else{
            string s;
            std::cin >> s;
            bool f = true;
            range(i, 0, 2 * n){
                if((s[i] == 'R') ^ v[i]){
                    f = false;
                    break;
                }
            }
            assert(f);
            return 0;
        }
    }
}