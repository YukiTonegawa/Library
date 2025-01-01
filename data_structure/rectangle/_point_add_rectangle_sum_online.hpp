template<typename Idx, typename Val, int max_n_log = 30>
struct online_point_add_rectangle_sum{
private:
  struct node{
    map_sum_cache<Idx, Val> mp;
    std::vector<std::pair<Idx, Val>> initial_points; // 初期化用　
    node *l, *r;
    node(): l(nullptr), r(nullptr){}
  };
  node *root;
  void update_inner(Idx x, Idx y, Val z){
    node *v = root;
    Idx lx = 0, rx = (Idx)1 << max_n_log;
    while(true){
      Idx mx = (lx + rx) / 2;
      if(rx <= x){
        if(!v->r) v->r = new node();
        v = v->r;
        lx = rx, rx += rx - mx;
      }else{
        v->mp.update(y, z);
        if(rx - 1 == x) return;
        rx = mx;
        if(!v->l) v->l = new node();
        v = v->l;
      }
    }
  }
  Val query_inner(Idx x, Idx ly, Idx ry){
    Idx lx = 0, rx = (Idx)1 << max_n_log;
    Val ret = 0;
    node *v = root;
    while(v){
      Idx mx = (lx + rx) / 2;
      if(rx <= x){
        ret += v->mp.query(ly, ry);
        if(rx == x) return ret;
        v = v->r;
        lx = rx;
        rx += rx - mx;
      }else{
        v = v->l;
        rx = mx;
      }
    }
    return ret;
  }
  using point = std::tuple<Idx, Idx, Val>;
public:
  online_point_add_rectangle_sum(): root(new node()){}
  online_point_add_rectangle_sum(std::vector<point> v): root(new node()){
    sort(v.begin(), v.end(), [](const point &a, const point &b){
      return std::get<1>(a) < std::get<1>(b);
    });
    auto push = [&](Idx x, Idx y, Val z){
      node *v = root;
      Idx lx = 0, rx = (Idx)1 << max_n_log;
      while(true){
        Idx mx = (lx + rx) / 2;
        if(rx <= x){
          if(!v->r) v->r = new node();
          v = v->r;
          lx = rx, rx += rx - mx;
        }else{
          if(v->initial_points.empty() || v->initial_points.back().first != y){
            v->initial_points.push_back({y, z});
          }else{
            v->initial_points.back().second += z;
          }
          if(rx - 1 == x) return;
          rx = mx;
          if(!v->l) v->l = new node();
          v = v->l;
        }
      }
    };
    for(auto [x, y, z] : v) push(x, y, z);
    auto init = [&](auto &&init, node *v) -> void {
      v->mp.init_sorted(v->initial_points);
      v->initial_points.clear();
      if(v->l) init(init, v->l);
      if(v->r) init(init, v->r);
    };
    init(init, root);
  }
  void update(Idx x, Idx y, Val z){
    update_inner(x, y, z);
  }
  Val query(Idx lx, Idx rx, Idx ly, Idx ry){
    return query_inner(rx, ly, ry) - query_inner(lx, ly, ry);
  }
};
