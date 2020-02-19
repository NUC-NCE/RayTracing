#ifndef RENDERER_H
#define RENDERER_H

#include <QObject>
#include <cmath>
#include <QPixmap>
#include <QImage>
#include <QDebug>
#include "utility.h"
struct Vec {//定义向量类型
    double x, y, z;

    Vec(double xx = 0, double yy = 0, double zz = 0) {
        x = xx;
        y = yy;
        z = zz;
    }

    Vec operator +(const Vec &b) const {
        return Vec(x + b.x, y + b.y, z + b.z);
    }

    Vec operator -(const Vec &b) const {
        return Vec(x - b.x, y - b.y, z - b.z);
    }

    Vec operator *(double b) const {
        return Vec(x * b, y * b, z * b);
    }

    Vec mult(const Vec &b) const {
        return Vec(x * b.x, y * b.y, z * b.z);
    }                                                                          //向量分量相乘

    Vec &norm() {
        return *this = *this * (1 / sqrt(x * x + y * y + z * z));
    }                                                                             //单位化本向量

    double dot(const Vec &b) const {
        return x * b.x + y * b.y + z * b.z;
    }                                                                          //点乘,

    Vec operator %(Vec &b) {
        return Vec(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
    }                                                                                                  //叉乘

};

struct Ray {
    Vec o, d;
    Ray(Vec o_, Vec d_) : o(o_), d(d_) {
    }

};

enum Refl_t {//材质定义
    DIFF, SPEC, REFR
};
struct Sphere {//定义圆数据结构
    double rad;
    Vec p, e, c;          // 位置,发射亮度,颜色
    Refl_t refl;          // 材质类型
    Sphere(double rad_, Vec p_, Vec e_, Vec c_, Refl_t refl_) :
        rad(rad_), p(p_), e(e_), c(c_), refl(refl_) {
    }

        //测试光线与圆是否相交,未相交返回0
    double intersect(Ray &r) {
        Vec op = p - r.o;
        double t, eps = 1e-4, b = op.dot(r.d), det = b * b - op.dot(op) + rad * rad;
        if (det < 0) return 0; else det = sqrt(det);
        return (t = b - det) > eps ? t : ((t = b + det) > eps ? t : 0);
    }

};


class Renderer : public QObject
{
  Q_OBJECT
public:
  explicit Renderer(QObject *parent = nullptr);
  QImage resultImg;
  //场景内的圆,墙面实际 用一个很大的圆模拟,参数依次是:半径,位置,发射光,颜色,材质类型
  Sphere spheres[12] = {//Scene: radius, position, emission, color, material
    // center 50 40.8 62
    // floor 0
    // back  0
    //     rad       pos                   emis           col     refl
  //  Sphere(1e3,   Vec(1,1,-2)*1e4,    Vec(1,1,1)*5e2,     Vec(), DIFF), // moon
  //  Sphere(3e2,   Vec(.6,.2,-2)*1e4,    Vec(1,1,1)*5e3,     Vec(), DIFF), //
  //  moon

    Sphere(2.5e3,   Vec(.82,.92,-2)*1e4,    Vec(1,1,1)*.8e2,     Vec(), DIFF), // moon

  //  Sphere(2.5e4, Vec(50, 0, 0),     Vec(1,1,1)*1e-3,    Vec(.2,.2,1)*0.0075, DIFF), // sky
  //  Sphere(2.5e4, Vec(50, 0, 0),  Vec(0.114, 0.133, 0.212)*1e-2,  Vec(.216,.384,1)*0.0007, DIFF), // sky

    Sphere(2.5e4, Vec(50, 0, 0),  Vec(0.114, 0.133, 0.212)*1e-2,  Vec(.216,.384,1)*0.003, DIFF), // sky

    Sphere(5e0,   Vec(-.2,0.16,-1)*1e4, Vec(1.00, 0.843, 0.698)*1e2,   Vec(), DIFF),  // star
    Sphere(5e0,   Vec(0,  0.18,-1)*1e4, Vec(1.00, 0.851, 0.710)*1e2,  Vec(), DIFF),  // star
    Sphere(5e0,   Vec(.3, 0.15,-1)*1e4, Vec(0.671, 0.780, 1.00)*1e2,   Vec(), DIFF),  // star
    Sphere(3.5e4,   Vec(600,-3.5e4+1, 300), Vec(),   Vec(.6,.8,1)*.01,  REFR),   //pool
    Sphere(5e4,   Vec(-500,-5e4+0, 0),   Vec(),      Vec(1,1,1)*.35,  DIFF),    //hill
    Sphere(16.5,  Vec(27,0,47),         Vec(),              Vec(1,1,1)*.33, DIFF), //hut
    Sphere(7,     Vec(27+8*sqrt(2),0,47+8*sqrt(2)),Vec(),  Vec(1,1,1)*.33,  DIFF), //door
    Sphere(500,   Vec(-1e3,-300,-3e3), Vec(),  Vec(1,1,1)*.351,    DIFF),  //mnt
    Sphere(830,   Vec(0,   -500,-3e3), Vec(),  Vec(1,1,1)*.354,    DIFF),  //mnt
    Sphere(490,  Vec(1e3,  -300,-3e3), Vec(),  Vec(1,1,1)*.352,    DIFF),  //mnt
  };
  inline double clamp(double x) {
      return x < 0 ? 0 : x > 1 ? 1 : x;
  }

  inline int toInt(double x) {
      return int(pow(clamp(x), 1 / 2.2) * 255 + .5);
  }

  inline bool intersect(Ray &r, double &t, int &id) {
      double n = sizeof(spheres) / sizeof(Sphere), d, inf = t = 1e20;
      for (int i = int(n); i--; )
          if ((d = spheres[i].intersect(r)) && d < t) {
              t = d;
              id = i;
          }
      return t < inf;
  }
  Vec radiance(Ray &&r, int depth, unsigned short *Xi);
  Vec radiance(Ray &r, int depth, unsigned short *Xi);
signals:
  void newLineRendered(QImage image);
  void renderState(int progress,int remain);
public slots:
  void renderScene(int sample,int width,int height);
};

#endif // RENDERER_H
