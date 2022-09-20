#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "hittable.h"
#include "vec3.h"
#include "aabb.h"
#include "material.h"
class constant_medium : public hittable {
 public:
  constant_medium(shared_ptr<hittable> b, double d, shared_ptr<texture> a)
      : boundary(b), neg_inv_density(-1 / d) {
    phase_function = make_shared<isotropic>(a);
    //对于散射的方向来说，我们采用各项同性(isotropic)的随机单位向量大法
  }

  virtual bool hit(const ray& r,
                   double t_min,
                   double t_max,
                   hit_record& rec) const;

  virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
    return boundary->bounding_box(t0, t1, output_box);
  }

 public:
  shared_ptr<hittable> boundary;
  shared_ptr<material> phase_function;
  double neg_inv_density;//体积体的光学密度比例常数
};
bool constant_medium::hit(const ray& r,
                          double t_min,
                          double t_max,
                          hit_record& rec) const {
  // Print occasional samples when debugging. To enable, set enableDebug true.
  const bool enableDebug = false;
  const bool debugging = enableDebug && random_double() < 0.00001;

  hit_record rec1, rec2;

  if (!boundary->hit(r, -infinity, infinity, rec1))
    return false;

  if (!boundary->hit(r, rec1.t + 0.0001, infinity, rec2))
    return false;

  if (debugging)
    std::cerr << "\nt0=" << rec1.t << ", t1=" << rec2.t << '\n';

  if (rec1.t < t_min)
    rec1.t = t_min;
  if (rec2.t > t_max)
    rec2.t = t_max;

  if (rec1.t >= rec2.t)
    return false;

  if (rec1.t < 0)
    rec1.t = 0;

  const auto ray_length = r.direction().length();
  const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
  const auto hit_distance = neg_inv_density * log(random_double());//这里有个log函数

  if (hit_distance > distance_inside_boundary)
    return false;

  rec.t = rec1.t + hit_distance / ray_length;
  rec.p = r.at(rec.t);

  if (debugging) {
    std::cerr << "hit_distance = " << hit_distance << '\n'
              << "rec.t = " << rec.t << '\n'
              << "rec.p = " << rec.p << '\n';
  }

  rec.normal = vec3(1, 0, 0);  // arbitrary（任意）
  rec.front_face = true;       // also arbitrary
  rec.mat_ptr = phase_function;

  return true;
}
//上述代码只能确保射线只会射入体积体一次, 之后再也不进入体积体的情况。换句话说,它假定
//体积体的边界是一个凸几何体。所以这个狭义的实现只对球体或者长方体这样的物体生效。但是
//对于当中有洞的那种形状,如甜甜圈就不行了
#endif