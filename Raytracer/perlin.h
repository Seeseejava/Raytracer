#pragma once
#include "vec3.h"
#include "rtweekend.h"


inline double trilinear_interp(vec3 c[2][2][2],
                               double u,
                               double v,
                               double w) {
  //平滑曲线的选择
  // cosine:y=(1 - cos(t * pi))/2
  // hermite cubic: y = 3t^2 - 2*t^3
  // Ken Perlin建议: y = 6*t^5 - 15*t^4 + 10* t^3;
  auto uu = u * u * (3 - 2 * u);
  auto vv = v * v * (3 - 2 * v);
  auto ww = w * w * (3 - 2 * w);
  auto accum = 0.0;
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
      for (int k = 0; k < 2; k++) {
        vec3 weight_v(u - i, v - j, w - k);//到顶点的距离
        accum += (i * uu + (1 - i) * (1 - uu)) * (j * vv + (1 - j) * (1 - vv)) *
                 (k * ww + (1 - k) * (1 - ww)) * dot(c[i][j][k], weight_v);
      }

  return accum;
}


class perlin {
  /*柏林噪声的关键特点是可复现性。如果输入的是同一个三维空间中的点,
      他的输出值总是相同的。柏林噪声的另一个特点是它实现起来简单快捷*/
 public:
  perlin() {
    /*现在看上去还是有一点格子的感觉,
        也许是因为这方法的最大值和最小值总是精确地落在了整数的x / y / z上,
        Ken Perlin有一个十分聪明的trick,
        在网格点使用随机的单位向量替代float(即梯度向量),
        用点乘将min和max值推离网格点,
        所以我们首先要把random floats改成random vectors。*/
    //ranfloat = new double[point_count];
    //for (int i = 0; i < point_count; ++i) {
    //  ranfloat[i] = random_double();//随机生成256个[0,1)的值
    //}
    ranvec = new vec3[point_count];

    for (int i = 0; i < point_count; ++i) {
      ranvec[i] = unit_vector(vec3::random(-1, 1));
    }
    perm_x = perlin_generate_perm();
    perm_y = perlin_generate_perm();
    perm_z = perlin_generate_perm();
    //生成permutation table 为了节省空间
  }

  ~perlin() {
    //delete[] ranfloat;
    delete[] ranvec;
    delete[] perm_x;
    delete[] perm_y;
    delete[] perm_z;
  }

  //double noise(const vec3& p) const {
  //  auto u = p.x() - floor(p.x());
  //  auto v = p.y() - floor(p.y());
  //  auto w = p.z() - floor(p.z());//取x,y,z的小数部分，就可以表示单元空间里的一个点

  //  auto i = static_cast<int>(4 * p.x()) & 255;
  //  auto j = static_cast<int>(4 * p.y()) & 255;
  //  auto k = static_cast<int>(4 * p.z()) & 255;//限制在255内避免数组越界错误
  //  //但是也产生了一个副作用，就是柏林噪声每隔256个整数就会再次重复
  //  //这里乘4应该是等价于改变了噪声的频率
  //  return ranfloat[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
  //  //将三个之和限制在255之内
  //}
  //更新后的版本
  double noise(const vec3& p) const {
    auto u = p.x() - floor(p.x());
    auto v = p.y() - floor(p.y());
    auto w = p.z() - floor(p.z());



    int i = floor(p.x());
    int j = floor(p.y());
    int k = floor(p.z());
    //double c[2][2][2];
    vec3 c[2][2][2];
    for (int di = 0; di < 2; di++)
      for (int dj = 0; dj < 2; dj++)
        for (int dk = 0; dk < 2; dk++)
          c[di][dj][dk] =
              /*ranfloat[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^
                       perm_z[(k + dk) & 255]];*/
              ranvec[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^
                       perm_z[(k + dk) & 255]];

    return trilinear_interp(c, u, v, w);
  }

  double turb(const vec3& p, int depth = 7) const {
    auto accum = 0.0;
    vec3 temp_p = p;
    auto weight = 1.0;

    for (int i = 0; i < depth; i++) {
      accum += weight * noise(temp_p);
      weight *= 0.5;
      temp_p *= 2;
    }

    return fabs(accum);//math.h里面的求绝对值
  }
  //使用多个频率相加得到复合噪声是一种很常见的做法,
  //    我们常常称之为扰动(turbulence),
  //    是一种由多次噪声运算的结果相加得到的产物
 private:
  static const int point_count = 256;
  //double* ranfloat;
  vec3* ranvec;
  int* perm_x;
  int* perm_y;
  int* perm_z;

  static int* perlin_generate_perm() {
    auto p = new int[point_count];

    for (int i = 0; i < perlin::point_count; i++)
      p[i] = i;

    permute(p, point_count);

    return p;
  }

  static void permute(int* p, int n) {
    for (int i = n - 1; i > 0; i--) {
      int target = random_int(0, i);
      int tmp = p[i];
      p[i] = p[target];
      p[target] = tmp;//打乱
    }
  }
};