
#include <iostream>

#include "rtweekend.h"
#include "sphere.h"
#include "hittable_list.h"
#include "camera.h"
#include "vec3.h"
#include "material.h"
#include "texture.h"
#include "stb_image.h"
#include "image_texture.h"
#include "xy_rect.h"
#include "box.h"
#include "flip_face.h"
#include "translate.h"
#include "rotate.h"
#include "constant_medium.h"
#include "bvh.h"


//bool hit_sphere(const vec3& center, double radius, const ray& r) {
//  vec3 oc = r.origin() - center;
//
//  auto a = r.direction().length_squared();
//  auto half_b = dot(oc, r.direction());
//  auto c = oc.length_squared() - radius * radius;
//  auto discriminant = half_b * half_b - a * c;
//
//  if (discriminant < 0) {
//    return -1.0;
//  } else {
//    return (-half_b - sqrt(discriminant)) / a;
//  }
//}

hittable_list cornell_box_diffuse() {
  hittable_list objects;

  auto red = make_shared<lambertian>(make_shared<constant_texture>(vec3(.65, .05, .05)));
  auto white = make_shared<lambertian>(make_shared<constant_texture>(vec3(.73, .73, .73)));
  auto green = make_shared<lambertian>(make_shared<constant_texture>(vec3(.12, .45, .15)));
  auto light = make_shared<diffuse_light>(make_shared<constant_texture>(vec3(15, 15, 15)));

  objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
  objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
  objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
  objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
  objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
  objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

  shared_ptr<hittable> box1 =
      make_shared<box>(vec3(0, 0, 0), vec3(165, 330, 165), white);
  box1 = make_shared<rotate_y>(box1, 15);
  box1 = make_shared<translate>(box1, vec3(265, 0, 295));
  objects.add(box1);

  shared_ptr<hittable> box2 =
      make_shared<box>(vec3(0, 0, 0), vec3(165, 165, 165), white);
  box2 = make_shared<rotate_y>(box2, -18);
  box2 = make_shared<translate>(box2, vec3(130, 0, 65));
  objects.add(box2);

  return objects;
}
hittable_list final_scene() {
  hittable_list boxes1;
  auto ground = make_shared<lambertian>(
      make_shared<constant_texture>(vec3(0.48, 0.83, 0.53)));

  const int boxes_per_side = 20;
  for (int i = 0; i < boxes_per_side; i++) {
    for (int j = 0; j < boxes_per_side; j++) {
      auto w = 100.0;
      auto x0 = -1000.0 + i * w;
      auto z0 = -1000.0 + j * w;
      auto y0 = 0.0;
      auto x1 = x0 + w;
      auto y1 = random_double(1, 101);
      auto z1 = z0 + w;

      boxes1.add(make_shared<box>(vec3(x0, y0, z0), vec3(x1, y1, z1), ground));
    }
  }

  hittable_list objects;

  objects.add(make_shared<bvh_node>(boxes1, 0, 1));

  auto light =
      make_shared<diffuse_light>(make_shared<constant_texture>(vec3(7, 7, 7)));
  objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light));

  auto center1 = vec3(400, 400, 200);
  auto center2 = center1 + vec3(30, 0, 0);
  auto moving_sphere_material = make_shared<lambertian>(
      make_shared<constant_texture>(vec3(0.7, 0.3, 0.1)));
  objects.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50,
                                         moving_sphere_material));

  objects.add(make_shared<sphere>(vec3(260, 150, 45), 50,
                                  make_shared<dielectric>(1.5)));
  objects.add(make_shared<sphere>(
      vec3(0, 150, 145), 50, make_shared<metal>(vec3(0.8, 0.8, 0.9), 10.0)));

  auto boundary = make_shared<sphere>(vec3(360, 150, 145), 70,
                                      make_shared<dielectric>(1.5));
  objects.add(boundary);
  objects.add(make_shared<constant_medium>(
      boundary, 0.2, make_shared<constant_texture>(vec3(0.2, 0.4, 0.9))));
  boundary =
      make_shared<sphere>(vec3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
  objects.add(make_shared<constant_medium>(
      boundary, .0001, make_shared<constant_texture>(vec3(1, 1, 1))));

  int nx, ny, nn;
  auto tex_data = stbi_load("E:/Texture/earthmap.jpg", &nx, &ny, &nn, 0);
  auto emat =
      make_shared<lambertian>(make_shared<image_texture>(tex_data, nx, ny));
  objects.add(make_shared<sphere>(vec3(400, 200, 400), 100, emat));
  auto pertext = make_shared<noise_texture>(0.1);
  objects.add(make_shared<sphere>(vec3(220, 280, 300), 80,
                                  make_shared<lambertian>(pertext)));

  hittable_list boxes2;
  auto white = make_shared<lambertian>(
      make_shared<constant_texture>(vec3(0.73, 0.73, 0.73)));
  int ns = 1000;
  for (int j = 0; j < ns; j++) {
    boxes2.add(make_shared<sphere>(vec3::random(0, 165), 10, white));
  }

  objects.add(make_shared<translate>(
      make_shared<rotate_y>(make_shared<bvh_node>(boxes2, 0.0, 1.0), 15),
      vec3(-100, 270, 395)));

  return objects;
}
hittable_list cornell_smoke() {
  hittable_list objects;

  auto red = make_shared<lambertian>(
      make_shared<constant_texture>(vec3(0.65, 0.05, 0.05)));
  auto white = make_shared<lambertian>(
      make_shared<constant_texture>(vec3(0.73, 0.73, 0.73)));
  auto green = make_shared<lambertian>(
      make_shared<constant_texture>(vec3(0.12, 0.45, 0.15)));
  auto light =
      make_shared<diffuse_light>(make_shared<constant_texture>(vec3(7, 7, 7)));

  objects.add(
      make_shared<flip_face>(make_shared<yz_rect>(0, 555, 0, 555, 555, green)));
  objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
  objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
  objects.add(
      make_shared<flip_face>(make_shared<xz_rect>(0, 555, 0, 555, 555, white)));
  objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
  objects.add(
      make_shared<flip_face>(make_shared<xy_rect>(0, 555, 0, 555, 555, white)));

  shared_ptr<hittable> box1 =
      make_shared<box>(vec3(0, 0, 0), vec3(165, 330, 165), white);
  box1 = make_shared<rotate_y>(box1, 15);
  box1 = make_shared<translate>(box1, vec3(265, 0, 295));

  shared_ptr<hittable> box2 =
      make_shared<box>(vec3(0, 0, 0), vec3(165, 165, 165), white);
  box2 = make_shared<rotate_y>(box2, -18);
  box2 = make_shared<translate>(box2, vec3(130, 0, 65));

  objects.add(make_shared<constant_medium>(
      box1, 0.01, make_shared<constant_texture>(vec3(0, 0, 0))));
  objects.add(make_shared<constant_medium>(
      box2, 0.01, make_shared<constant_texture>(vec3(1, 1, 1))));

  return objects;
}
hittable_list cornell_box() {
  hittable_list objects;

  auto red = make_shared<lambertian>(
      make_shared<constant_texture>(vec3(0.65, 0.05, 0.05)));
  auto white = make_shared<lambertian>(
      make_shared<constant_texture>(vec3(0.73, 0.73, 0.73)));
  auto green = make_shared<lambertian>(
      make_shared<constant_texture>(vec3(0.12, 0.45, 0.15)));
  auto light = make_shared<diffuse_light>(
      make_shared<constant_texture>(vec3(15, 15, 15)));

  objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
  objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
  objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
  objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
  objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));
  objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));

  shared_ptr<hittable> box1 =
  make_shared<box>(vec3(0, 0, 0), vec3(165, 330, 165), white);
  box1 = make_shared<rotate_y>(box1, 15);
  box1 = make_shared<translate>(box1, vec3(265, 0, 295));
  objects.add(box1);

  shared_ptr<hittable> box2 =
  make_shared<box>(vec3(0, 0, 0), vec3(165, 165, 165), white);
  box2 = make_shared<rotate_y>(box2, -18);
  box2 = make_shared<translate>(box2, vec3(130, 0, 65));
  objects.add(box2);
  return objects;
}
hittable_list simple_light() {
  hittable_list objects;

  auto pertext = make_shared<noise_texture>(4);
  objects.add(make_shared<sphere>(vec3(0, -1000, 0), 1000,
                                  make_shared<lambertian>(pertext)));
  objects.add(
      make_shared<sphere>(vec3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

  auto difflight =
      make_shared<diffuse_light>(make_shared<constant_texture>(vec3(4, 4, 4)));
  objects.add(make_shared<sphere>(vec3(0, 7, 0), 2, difflight));
  objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, difflight));

  return objects;
}
hittable_list random_scene() {
  hittable_list world;
  auto checker = make_shared<checker_texture>(
      make_shared<constant_texture>(vec3(0.2, 0.3, 0.1)),
      make_shared<constant_texture>(vec3(0.9, 0.9, 0.9)));

  world.add(make_shared<sphere>(vec3(0, -1000, 0), 1000,
                         make_shared<lambertian>(checker)));

  int i = 1;
  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      auto choose_mat = random_double();
      vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
      if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
        if (choose_mat < 0.8) {
          // diffuse
          auto albedo = vec3::random() * vec3::random();
          /*world.add(make_shared<sphere>(center, 0.2,
                                        make_shared<lambertian>(albedo)));*/
          world.add(make_shared<moving_sphere>(
              center, center + vec3(0, random_double(0, .5), 0), 0.0, 1.0, 0.2,
              make_shared<lambertian>(make_shared<constant_texture>(albedo))));
        } else if (choose_mat < 0.95) {
          // metal
          auto albedo = vec3::random(.5, 1);
          auto fuzz = random_double(0, .5);
          world.add(make_shared<sphere>(center, 0.2,
                                        make_shared<metal>(albedo, fuzz)));
        } else {
          // glass
          world.add(
              make_shared<sphere>(center, 0.2, make_shared<dielectric>(1.5)));
        }
      }
    }
  }

  world.add(
      make_shared<sphere>(vec3(0, 1, 0), 1.0, make_shared<dielectric>(1.5)));

  world.add(make_shared<sphere>(vec3(-4, 1, 0), 1.0,
                                make_shared<lambertian>(make_shared<constant_texture>( vec3(0.4, 0.2, 0.1)))));

  world.add(make_shared<sphere>(vec3(4, 1, 0), 1.0,
                                make_shared<metal>(vec3(0.7, 0.6, 0.5), 0.0)));

  return world;
}

hittable_list two_perlin_spheres() {
  hittable_list objects;

  auto pertext = make_shared<noise_texture>(5);
  objects.add(make_shared<sphere>(vec3(0, -1000, 0), 1000,
                                  make_shared<lambertian>(pertext)));
  objects.add(
      make_shared<sphere>(vec3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

  return objects;
}

hittable_list earth() {
  int nx, ny, nn;
  unsigned char* texture_data = stbi_load("E:/Texture/earthmap.jpg", &nx, &ny, &nn, 0);

  auto earth_surface =
      make_shared<lambertian>(make_shared<image_texture>(texture_data, nx, ny));
  auto globe = make_shared<sphere>(vec3(0, 0, 0), 2, earth_surface);

  return hittable_list(globe);
}

vec3 ray_color(const ray& r,
               const vec3& background,
               const hittable& world,
               int depth) {
  hit_record rec;

  if (depth <= 0)
    return vec3(0, 0, 0);

  // If the ray hits nothing, return the background color.
  if (!world.hit(r, 0.001, infinity, rec))
    return background;

  // if (world.hit(r, 0.01, infinity, rec))
  // {//这里是0.01而不是0，是为了防止自交问题，从而避免阴影痤疮(shadow ance）
  //    //有些物体反射的光线会在t=0时再次击中自己。然而由于精度问题,
  //    这个值可能是t=-0.000001或者是t=0.0000000001或者任意接近0的浮点数。所以我们要忽略掉0附近的一部分范围,
  //    防止物体发出的光线再次与自己相交。
  //  ray scattered;
  //  vec3 attenuation;
  //  if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
  //    return attenuation * ray_color(scattered, world, depth - 1);
  //  return vec3(0, 0, 0);
  //  //vec3 target = rec.p + rec.normal +
  //  random_unit_vector();//在求内随机生成一个点
  //  //return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth -
  //  1);//递归并衰减,这里为什么要进行递归
  //  ////return vec3(1, 0, 0);
  //}
  // vec3 unit_direction = unit_vector(r.direction());
  // auto t = 0.5 * (unit_direction.y() + 1.0);//y取(-1,1)从而t取得(0,1)
  // return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5,
  // 0.7, 1.0);//蓝白线性插值
  ray scattered;
  vec3 attenuation;
  vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
  double pdf;
  vec3 albedo;
  if (!rec.mat_ptr->scatter(r, rec, albedo, scattered, pdf))
    return emitted;
  //-------------注释------------------------------------
  auto on_light = vec3(random_double(213, 343), 554, random_double(227, 332));//面光源上随机一个点
  auto to_light = on_light - rec.p;
  auto distance_squared = to_light.length_squared();
  to_light = unit_vector(to_light);

  if (dot(to_light, rec.normal) < 0)
    return emitted;

  double light_area = (343 - 213) * (332 - 227);
  auto light_cosine = fabs(to_light.y());//α为pq连线与A法线的夹角
  if (light_cosine < 0.000001)
    return emitted;

  pdf = distance_squared / (light_cosine * light_area);//公式推导出来的
  scattered = ray(rec.p, to_light, r.time());
  //-------------注释------------------------------------
  return emitted + albedo * rec.mat_ptr->scattering_pdf(r, rec, scattered) *
                       ray_color(scattered, background, world, depth - 1) / pdf;
}
int main() {
  const auto aspect_ratio = 1.0 / 1.0;
  const int image_width = 600;
  const int image_height = static_cast<int>(image_width / aspect_ratio);
  const int samples_per_pixel = 10;
  const int max_depth = 50;
  const vec3 background(0, 0, 0);

  std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
  vec3 lower_left_corner(-2.0, -1.0, -1.0);
  vec3 horizontal(4.0, 0.0, 0.0);
  vec3 vertical(0.0, 2.0, 0.0);
  vec3 origin(0.0, 0.0, 0.0);//4个点分别为（-2，-1，-1），（2，-1，-1），（2，1，-1），（-2，1，-1）

  
//  world.add(
//      make_shared<sphere>(vec3(-1, 0, -1), 0.5, make_shared<dielectric>(1.5)));
//  world.add(make_shared<sphere>(vec3(-1, 0, -1), -0.45,
//                                make_shared<dielectric>(1.5)));//这里有个简单又好用的trick, 如果你将球的半径设为负值, 形状看上去并没什么变化, 
////但是法相全都翻转到内部去了。所以就可以用这个特性来做出一个通透的玻璃球:【把一个小球套在大球里, 光线发生两次折射, 于是负负得正, 上下不会颠倒】



  //vec3 lookfrom(3, 3, 2);
  //vec3 lookat(0, 0, -1);
  //vec3 vup(0, 1, 0);
  //auto dist_to_focus = (lookfrom - lookat).length();
  //auto aperture = 2.0;

  //camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

  //auto world = random_scene();
  //auto world = two_perlin_spheres();
  //auto world = earth();



  /*auto world = simple_light();
  vec3 lookfrom(23, 2, 3);
  vec3 lookat(0, 0, 0);
  vec3 vup(0, 1, 0);
  auto dist_to_focus = 10.0;
  auto aperture = 0.1;

  camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);*/

  //cornell_box摄像机参数
  //auto world = cornell_box();
  
  //auto world = cornell_smoke();
  
  auto world = cornell_box_diffuse();
  //auto world = final_scene();
  vec3 lookfrom(278, 278, -800);
  vec3 lookat(278, 278, 0);
  vec3 vup(0, 1, 0);
  auto dist_to_focus = 10.0;
  auto aperture = 0.0;
  auto vfov = 40.0;

  camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus,
             0.0, 1.0);
  for (int j = image_height - 1; j >= 0; --j) {
    std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
    for (int i = 0; i < image_width; ++i) {
        //从左到右，从上到下输入
      vec3 color(0, 0, 0);
      
      for (int s = 0; s < samples_per_pixel; ++s) {
        auto u = (i + random_double()) / image_width;
        auto v = (j + random_double()) / image_height;
        ray r = cam.get_ray(u, v);
        color += ray_color(r, background, world, max_depth);
      }
      color.write_color(std::cout, samples_per_pixel);
    }
  }
  
}