#ifndef MATERIAL_H
#define MATERIAL_H
//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include "ray.h"
#include "rtweekend.h"
#include <cmath>


inline double reflectance(double cosine, double refraction_index) {
    // Use Schlick's approximation for reflectance.
    auto r0 = (1 - refraction_index) / (1 + refraction_index);
    r0 = r0*r0;
    return r0 + (1-r0)*std::pow((1 - cosine),5);
}
inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2*dot(v,n)*n;
}

inline vec3 refract(const vec3& uv, const vec3& n, float etai_over_etat) {
    float cos_theta = std::fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp =  etai_over_etat * (uv + n * cos_theta);
    vec3 r_out_parallel = -std::sqrtf(std::fabs(1.0 - dot(r_out_perp, r_out_perp))) * n;
    return r_out_perp + r_out_parallel;
}


enum material_type {LAMBERT, METAL, DIELECTRIC};
struct material {
  material_type type;
  union {
    struct lambertian { color albedo; } l;
    struct metal { color albedo; float fuzz;} m;
    struct dielectric { float refraction_index;} d;
  } data;


  //constructions
  static material make_lambert(color albedo) {
    material mat{};
    mat.type = LAMBERT;
    mat.data.l.albedo = albedo;
    return mat;
  };
  static material make_metal(color albedo, double fuzz) {
    material mat{};
    mat.type = METAL;
    mat.data.m.albedo = albedo;
    mat.data.m.fuzz = fuzz;
    return mat;
  };
  static material make_dielectric(double rIdx) {
    material mat{};
    mat.type = DIELECTRIC;
    mat.data.d.refraction_index = rIdx;
    return mat;
  };
  




  inline bool scatter(ray& r_in, color& attenuation) {
    switch (type) {
      case(LAMBERT): return lambertScatter(r_in, attenuation); break;
      case(DIELECTRIC): return dielectricScatter(r_in, attenuation); break;
      case(METAL): return metalScatter(r_in, attenuation); break;
      default: return false;
    }
  }
  inline bool lambertScatter(ray& r, color& attenuation) {
    auto scatter_direction = r.normal + random_unit_vector();
    // Catch degenerate scatter direction

    r.dir = scatter_direction;
    attenuation = data.l.albedo;
    return true;
  }
  inline bool metalScatter(ray& r, color& attenuation) {
    vec3 reflected = reflect(r.dir, r.normal);
    reflected = unit_vector(reflected) + (data.m.fuzz * random_unit_vector());

    r.dir = reflected;
    attenuation = data.m.albedo;
    return (dot(r.dir, r.normal) > 0);
  }
  inline bool dielectricScatter(ray& r, color& attenuation) {
    attenuation = color(1.0, 1.0, 1.0);
    auto & refraction_index = data.d.refraction_index;
    bool front_face = dot(r.dir, r.normal) < 0;
    double ri = front_face ? (1.0/refraction_index) : refraction_index;
    auto normal = front_face ? r.normal : - r.normal;

    vec3 unit_direction = unit_vector(r.dir);
    double cos_theta = std::fmin(dot(-unit_direction,normal), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

    bool cannot_refract = ri * sin_theta > 1.0;
    vec3 direction;

    if (cannot_refract || reflectance(cos_theta, ri) > random_float())
        direction = reflect(unit_direction, normal);
    else
        direction = refract(unit_direction, normal, ri);

    r.dir = direction;
    return true;
  }

};


#endif
