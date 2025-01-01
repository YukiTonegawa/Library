/*
//f_i(x) := 1 ± a_i x^b_i を大量に掛ける
//O(N^2/th + thMlogM)
// N = M = 1e5 なら50が早かった
template<ll MOD>
StaticModFPS<MOD> multiply_binomial(vector<vector<ll>> v, ll m, ll th){
  using fps = StaticModFPS<MOD>;
  fps ret_big(m, 0);
  vvl big;
  vector<vector<fps>> small(th+1, vector<fps>());
  vector<ll> inv((m/th)+2);
  inv[0] = inv[1] = 1;
  for(ll i=2;i<=(m/th)+1;i++){
    ll u = MOD - (MOD/i);
    inv[i] = (u*inv[MOD%i])%MOD;
  }
  for(auto e:v){
    e[0] %= MOD;
    if(e[0] < 0) e[0] += MOD;
    if(e[1]>th) big.push_back(e);
    else small[e[1]].push_back({1, e[0]});
  }
  for(int i=0;i<big.size();i++){
    ll j = 1;
    ll mul = big[i][0];
    while(j * big[i][1] < m){
      ll y;
      if(j%2==0) y = ((MOD - abs(mul)) * inv[j])%MOD;
      else y = (abs(mul) * inv[j])%MOD;
      ret_big[j * big[i][1]] = (ret_big[j * big[i][1]] + y)%MOD;
      mul = (mul * big[i][0])%MOD;
      j++;
    }
  }
  ret_big = ret_big.exp(m);
  for(ll i=1;i<=th;i++){
    if(small[i].size()==0) continue;
    fps tmp = multiply_lower_degree<MOD>(small[i], m/i+1);
    bool f = true;

    for(int j=m-1;j>=1;j--){
      if(f&&j%i==0&&tmp.size()>(j/i)&&tmp[j/i]){
        tmp.resize(j+1);
        break;
      }
    }
    for(int j=tmp.size()-1;j>=1;j--){
      if(j%i==0) tmp[j] = tmp[j/i];
      else tmp[j] = 0;
    }
    ret_big *= tmp;
    if(ret_big.size()>m) ret_big.resize(m);
  }
  return ret_big;
}
*/
/*
//1 ± x^a_i を大量に掛ける
template<ll MOD>
StaticModFPS<MOD> multiply_binomial_one(const vector<vector<ll>> &v, ll m){
  using fps = StaticModFPS<MOD>;
  vector<ll> pl(m, 0), mi(m, 0);
  for(int i=0;i<v.size();i++){
    if(v[i][0]==-1) mi[v[i][1]]++;
    else pl[v[i][1]]++;
  }
  vector<ll> inv(m+1);
  inv[0] = inv[1] = 1;
  for(ll i=2;i<=m;i++){
    ll u = MOD - (MOD/i);
    inv[i] = (u*inv[MOD%i])%MOD;
  }
  fps ret(m, 0);
  for(ll i=1;i<m;i++){
    ll j = 1;
    while(j*i<m){
      ll mlt = (j%2==0?(-pl[i]-mi[i]):pl[i]-mi[i]);
      mlt = (mlt * inv[j])%MOD;
      mlt = (mlt + MOD)%MOD;
      ret[i*j] += mlt;
      if(ret[i*j]>=MOD) ret[i*j] -= MOD;
      j++;
    }
  }
  return ret.exp(m);
}
*/
/*
#endif
*/
