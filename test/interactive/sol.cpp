#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <queue>
#include <algorithm>
#include <set>
#include <map>
#include <bitset>
#include <cmath>
#include <functional>
#include <iomanip>
#define vll vector<ll>
#define vvv vector<vvl>
#define vvi vector<vector<int> >
#define vvl vector<vector<ll> >
#define vv(a, b, c, d) vector<vector<d> >(a, vector<d>(b, c))
#define vvvl(a, b, c, d) vector<vvl>(a, vvl(b, vll (c, d)));
#define rep(c, a, b) for(ll c=a;c<b;c++)
#define re(c, b) for(ll c=0;c<b;c++)
#define all(obj) (obj).begin(), (obj).end()
typedef long long int ll;
typedef long double ld;
using namespace std;

ll n;
void go(int i){
  string s = "? ";
  for(int j=0;j<n;j++) {
    s += to_string(i+j);
    if(j!=n-1) s += " ";
  }
  std::cout << s << "\n" << std::flush;
}

string make(int i){
  string s = "? ";
  for(int j=0;j<n-1;j++) s += to_string(i+j) + (j!=n-1?" ":"");
  return s;
}

int main(int argc, char const *argv[]) {
  std::cin >> n;
  string t;
  ll L = 1, R = n+1;
  go(1);
  string FIRST;
  std::cin >> FIRST;

  while(R-L>1){
    ll mid = (L+R)/2;
    go(mid);
    string s = "? ";
    std::cin >> t;
    if(t==FIRST) L = mid;
    else R = mid;
  }
  string x = make(R);

  vll ans(2*n+1, 0);
  vll r, b;

  for(int i=1;i<R;i++){
    string y = x + to_string(i);
    std::cout << y << "\n" << std::flush;
    std::cin >> t;
    if(t=="Red") r.push_back(i), ans[i] = 1;
    else b.push_back(i), ans[i] = 0;
  }
  for(int i=R+n-1;i<=2*n;i++){
    string y = x + to_string(i);
    std::cout << y << "\n" << std::flush;
    std::cin >> t;
    if(t=="Red") r.push_back(i), ans[i] = 1;
    else b.push_back(i), ans[i] = 0;
  }

  string z = "? ";
  for(int i=0;i<n/2;i++) z += to_string(r[i]) + " ";
  for(int i=0;i<n/2;i++) z += to_string(b[i]) + " ";

  // mid
  for(int i=R;i<R+n-1;i++){
    string tmp = z + to_string(i);
    std::cout << tmp << "\n" << std::flush;
    std::cin >> t;
    if(t=="Red") ans[i] = 1;
  }
  string ret = "! ";
  for(int i=1;i<=2*n;i++) ret += (ans[i]?"R":"B");
  std::cout << ret << "\n" << std::flush;
  return 0;
}
