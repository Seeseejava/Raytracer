#ifndef MATERIAL_H
#define MATERIAL_H


#include "hittable.h"
#include "texture.h"
#include "onb.h"
class material {
 public:
  virtual vec3 emitted(double u, double v, const vec3& p) const {
    return vec3(0, 0, 0);
  }
  virtual bool scatter(const ray& r_in,
                       const hit_record& rec,
                       vec3& albedo,
                       ray& scattered,
                       double& pdf) const {
    return false;  // ֮ǰ�Ǵ��麯���������޸�Ϊ����false
  }
  // --------------------������ 
  virtual double scattering_pdf(
        const ray& r_in,
        const hit_record& rec,
        const ray& scattered) const {
    return 0;
  }
  // --------------------


};



class lambertian : public material {
 public:
  
  lambertian(shared_ptr<texture> a) : albedo(a) {}

  //virtual bool scatter(const ray& r_in,
  //                     const hit_record& rec,
  //                     vec3& alb,
  //                     ray& scattered,
  //                     double& pdf) const override {
  //  vec3 scatter_direction = rec.normal + random_unit_vector();
  //  // Catch degenerate scatter direction
  //  /*if (scatter_direction.near_zero())
  //    scatter_direction = rec.normal;*/
  //  scattered = ray(rec.p, unit_vector(scatter_direction), r_in.time());
  //  alb = albedo->value(rec.u, rec.v, rec.p);
  //  pdf = dot(rec.normal, scattered.direction()) / pi;
  //  return true;
  //}
  //��һ�ֲ���
  /*virtual bool scatter(const ray& r_in,
                       const hit_record& rec,
                       vec3& alb,
                       ray& scattered,
                       double& pdf) const override {
    auto direction = random_in_hemisphere(rec.normal);
    scattered = ray(rec.p, unit_vector(direction), r_in.time());
    alb = albedo->value(rec.u, rec.v, rec.p);
    pdf = 0.5 / pi;
    return true;
  }*/

  virtual bool scatter(const ray& r_in,
                       const hit_record& rec,
                       vec3& alb,
                       ray& scattered,
                       double& pdf) const override {
    onb uvw;
    uvw.build_from_w(rec.normal);
    auto direction = uvw.local(random_cosine_direction());
    //���ɷ��߽���������ת��Ϊ��������
    scattered = ray(rec.p, unit_vector(direction), r_in.time());
    alb = albedo->value(rec.u, rec.v, rec.p);
    pdf = dot(uvw.w(), scattered.direction()) / pi;
    return true;
  }
  double scattering_pdf(const ray& r_in,
                        const hit_record& rec,
                        const ray& scattered) const {
    auto cosine = dot(rec.normal, unit_vector(scattered.direction()));
    return cosine < 0 ? 0 : cosine / pi;
  }

 public:
  shared_ptr<texture> albedo;
};

class metal : public material {
 public:
  metal(const vec3& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

  virtual bool scatter(const ray& r_in,
                       const hit_record& rec,
                       vec3& attenuation,
                       ray& scattered) const {
    vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
    scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
    attenuation = albedo;
    return (dot(scattered.direction(), rec.normal) > 0);
  }

 public:
  vec3 albedo;
  double fuzz;
  //���ǻ����Ը����䷽�����һ��������, ֻҪ���������������,�����յ�Ϊ���ĵ��������ѡȡһ������Ϊ���յ��յ�:
  //����һ����������ʾģ���ĳ̶�(fuzziness)(���Ե�fuzz=0ʱ�������ģ��)�����fuzz, Ҳ���������İ뾶�ܴ�, ���߿��ܻ�ɢ�䵽�����ڲ�ȥ��
};

class dielectric : public material {
 public:
  dielectric(double ri) : ref_idx(ri) {}

  virtual bool scatter(const ray& r_in,
                       const hit_record& rec,
                       vec3& attenuation,
                       ray& scattered) const {
    attenuation = vec3(1.0, 1.0, 1.0);//�������治���չ������
    double etai_over_etat = (rec.front_face) ? (1.0 / ref_idx) : (ref_idx);//��ϵ�Ƿ�ӽ��ʽ������

    vec3 unit_direction = unit_vector(r_in.direction());
    double cos_theta = ffmin(dot(-unit_direction, rec.normal), 1.0);
    double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
    if (etai_over_etat * sin_theta > 1.0) {
      vec3 reflected = reflect(unit_direction, rec.normal);
      scattered = ray(rec.p, reflected);
      return true;
    }
    double reflect_prob = schlick(cos_theta, etai_over_etat);
    if (random_double() < reflect_prob) {
      vec3 reflected = reflect(unit_direction, rec.normal);
      scattered = ray(rec.p, reflected);
      return true;
    }
    vec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
    scattered = ray(rec.p, refracted);
    return true;
  }

  double ref_idx;
};

class diffuse_light : public material {
 public:
  diffuse_light(shared_ptr<texture> a) : emit(a) {}

  virtual bool scatter(const ray& r_in,
                       const hit_record& rec,
                       vec3& attenuation,
                       ray& scattered) const {
    return false;
  }

  virtual vec3 emitted(double u, double v, const vec3& p) const {
    return emit->value(u, v, p);
  }

 public:
  shared_ptr<texture> emit;
};

class isotropic : public material {
 public:
  isotropic(shared_ptr<texture> a) : albedo(a) {}

  virtual bool scatter(const ray& r_in,
                       const hit_record& rec,
                       vec3& attenuation,
                       ray& scattered) const {
    scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
    attenuation = albedo->value(rec.u, rec.v, rec.p);
    return true;
  }

 public:
  shared_ptr<texture> albedo;
};
#endif