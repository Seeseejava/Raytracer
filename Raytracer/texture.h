#pragma once
#include "rtweekend.h"
#include "perlin.h"
class texture {
 public:
  virtual vec3 value(double u, double v, const vec3& p) const = 0;
};

class constant_texture : public texture {
 public:
  constant_texture() {}
  constant_texture(vec3 c) : color(c) {}

  virtual vec3 value(double u, double v, const vec3& p) const { return color; }

 public:
  vec3 color;
};

class checker_texture : public texture {
 public:
  checker_texture() {}
  checker_texture(shared_ptr<texture> t0, shared_ptr<texture> t1)
      : even(t0), odd(t1) {}

  virtual vec3 value(double u, double v, const vec3& p) const {
    auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
    if (sines < 0)
      return odd->value(u, v, p);
    else
      return even->value(u, v, p);
  }

 public:
  shared_ptr<texture> odd;
  shared_ptr<texture> even;
};



class noise_texture : public texture {
 public:
  noise_texture() {}
  noise_texture(double sc) : scale(sc) {}

  virtual vec3 value(double u, double v, const vec3& p) const {
    //return vec3(1, 1, 1) * 0.5 *(1.0 + noise.noise(scale *p));  //�����scaleʵ�����Ǹı�Ƶ�ʣ�scaleԽ��Ƶ��Խ��
    ////���ֲ�ֵ���������п����Ǹ���, ��Щ������٤��У��ʱ������ƽ����`sqrt()`����NaN�����ǽ�������ӳ�䵽0��1֮��
    return vec3(1, 1, 1) * 0.5 * (1 + sin(scale * p.z() + 10 * noise.turb(p)));
    /*����˼·������ɫ��sine������ֵ�ɱ���,��ʹ���Ŷ�����ȥ������λ(ƽ����sin(x) �е�x),
    ʹ�ô�״���������������������ֱ��ʹ���Ŷ�turb��������noise����ɫ��ֵ�ķ�����
    ���ǻ�õ�һ�����ƴ���ʯ������*/
  }

 public:
  perlin noise;
  double scale;
};