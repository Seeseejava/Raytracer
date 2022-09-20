#pragma once
#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>

// Usings

using std::make_shared;
using std::shared_ptr;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
  return degrees * pi / 180;
}

inline double ffmin(double a, double b) {
  return a <= b ? a : b;
}
inline double ffmax(double a, double b) {
  return a >= b ? a : b;
}

// Common Headers
//产生随机数
inline double random_double() {
  // Returns a random real in [0,1).
  return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
  // Returns a random real in [min,max).
  return min + (max - min) * random_double();
}

inline int random_int(int min, int max) {
  return rand() % (max - min) + min;
}

inline double clamp(double x, double min, double max) {
  if (x < min)
    return min;
  if (x > max)
    return max;
  return x;
}
//#include <functional>
//#include <random>
//
//inline double random_double() {
//  static std::uniform_real_distribution<double> distribution(0.0, 1.0);
//  static std::mt19937 generator;
//  static std::function<double()> rand_generator =
//      std::bind(distribution, generator);
//  return rand_generator();
//}也可以采取这种方法
#include "ray.h"
#include "vec3.h"

#endif