#ifndef FLIP_FACE_H
#define FLIP_FACE_H


#include "hittable.h"
#include "vec3.h"
#include "aabb.h"
#include "xy_rect.h"
#include "hittable_list.h"
//单纯用来翻转正反面
class flip_face : public hittable {
 public:
  flip_face(shared_ptr<hittable> p) : ptr(p) {}

  virtual bool hit(const ray& r,
                   double t_min,
                   double t_max,
                   hit_record& rec) const {
    if (!ptr->hit(r, t_min, t_max, rec))
      return false;

    rec.front_face = !rec.front_face;
    return true;
  }

  virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
    return ptr->bounding_box(t0, t1, output_box);
  }

 public:
  shared_ptr<hittable> ptr;
};
#endif