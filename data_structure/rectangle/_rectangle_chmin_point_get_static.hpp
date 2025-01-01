/*
// 長方形がN個ありそれぞれ値が割り振られている, 各クエリでは以下の操作を行う
// q(x, y) := (x, y)を含む長方形に書かれている数字のうち最小のものを答える
template<typename Idx, typename compare_struct>
struct offline_static_rectangle_chmin_point_get{
  using Val = typename compare_struct::Val;
private:
  struct Rect{
    Idx lx, rx, ly, ry;
    Val z;
  };
  std::vector<Rect> R;
  kd_tree<Idx, int> kdt;
  using Point = typename kd_tree<Idx, int>::point;
  std::vector<Point> P;
public:
  void update(Idx lx, Idx rx, Idx ly, Idx ry, Val z){
    R.push_back({lx, rx, ly, ry, z});
  }
  void query(Idx x, Idx y){
    int qcnt = P.size();
    P.push_back(Point(x, y, qcnt));
  }
  // 追加される長方形を値の小さい順に見る
  // これがある質問点を含む場合, その質問点を消していい(それ以降の長方形はより値が大きいため)
  std::vector<Val> solve(){
    kdt = kd_tree<Idx, int>(P);
    std::sort(R.begin(), R.end(), [&](const Rect &a, const Rect &b){
      return compare_struct::compare(a.z, b.z);
    });
    std::vector<Val> ans(P.size(), compare_struct::id());
    for(Rect &r : R){
      auto V = kdt.range_find(r.lx, r.rx, r.ly, r.ry);
      for(auto v : V){
        ans[v->p.z] = r.z;
        kdt.erase(v);
      }
    }
    return ans;
  }
};
*/