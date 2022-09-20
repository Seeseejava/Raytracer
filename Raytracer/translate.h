#ifndef TRANSLATE_H
#define TRANSLATE_H


#include "hittable.h"
#include "vec3.h"
#include "aabb.h"
#include "xy_rect.h"
#include "hittable_list.h"
class translate : public hittable {
 public:
  translate(shared_ptr<hittable> p, const vec3& displacement)
      : ptr(p), offset(displacement) {}

  virtual bool hit(const ray& r,
                   double t_min,
                   double t_max,
                   hit_record& rec) const;
  virtual bool bounding_box(double t0, double t1, aabb& output_box) const;

 public:
  shared_ptr<hittable> ptr;
  vec3 offset;
};

bool translate::hit(const ray& r,
                    double t_min,
                    double t_max,
                    hit_record& rec) const {
  ray moved_r(r.origin() - offset, r.direction(), r.time());//射线原点位移
  if (!ptr->hit(moved_r, t_min, t_max, rec))
    return false;

  rec.p += offset;//注意这里要加上offset才是正确的射入点
  rec.set_face_normal(moved_r, rec.normal);

  return true;
}

bool translate::bounding_box(double t0, double t1, aabb& output_box) const {
  if (!ptr->bounding_box(t0, t1, output_box))
    return false;

  output_box = aabb(output_box.min() + offset, output_box.max() + offset);

  return true;
}
#endif