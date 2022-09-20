#pragma once
#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
 public:
  ray() {}
  ray(const vec3& origin, const vec3& direction, double time = 0.0) 
      : orig(origin),
      dir(direction), tm(time)  {}

  vec3 origin() const { return orig; }
  vec3 direction() const { return dir; }
  double time() const { return tm; }

  vec3 at(double t) const { return orig + t * dir; }

 public:
  vec3 orig;
  vec3 dir;
  double tm;
};

#endif

//(1) 将射线从视点转化为像素坐标
//(2) 计算光线是否与场景中的物体相交
//(3) 如果有,计算交点的颜色。
