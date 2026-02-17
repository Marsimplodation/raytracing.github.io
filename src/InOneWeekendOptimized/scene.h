#ifndef SCENE_H
#define SCENE_H


#include "material.h"
#include "ray.h"
#include "rtweekend.h"
#include <vector>
struct Spheres {
    //std::vector<float> radius;
    //std::vector<vec3> center;
    struct SphereGeom {
        vec3 center;
        float radius2;
    };
    struct InvGeom {
        vec3 v;
        float inv_radius;
    };
    std::vector<SphereGeom> geom;
    std::vector<InvGeom> invGeom;
    void add(vec3 v, float r) {
        float inv_radius = 1.0f / r;
        geom.push_back({.center = v, .radius2 = r * r});
        invGeom.push_back({.v = v * inv_radius, .inv_radius = inv_radius});
    }
};
Spheres& getSpheres();
std::vector<material>& getMaterials();


inline color background(ray &r){
    vec3 unit_direction = unit_vector(r.dir);
    float a = 0.5*(unit_direction.y + 1.0);
    return (1.0f-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);


}

inline color trace_ray(ray & r) {
    color attenuation(1,1,1);
    auto& spheres = getSpheres();
    auto* geom    = spheres.geom.data();
    auto* invGeom = spheres.invGeom.data();
    const int n   = (int)spheres.geom.size();

    auto& mats = getMaterials();
    int depth = 0;

    for (;;++depth) {
        const float KILLCHANCE = 0.15;
        const float xi = random_float();
        if (depth > 2) {
            if(xi < KILLCHANCE) {break;};
            attenuation *= 1.0f/((1.0f-KILLCHANCE));
        }
        const vec3 orig = r.orig;
        const vec3 dir  = r.dir;

        const float a     = dot(dir, dir);
        const float inv_a = 1.0f / a;

        int hitIdx = -1;
        r.it = interval(0.001, infinity);
        for (int i = 0; i < n; ++i) {
            const vec3 oc = geom[i].center - r.orig;
            unit_vector(oc);
            unit_vector(r.dir);
            const float h = dot(r.dir, oc);
            const float c = dot(oc, oc) - geom[i].radius2;

            const float disc = h*h - a*c;
            if (disc < 0.0f) continue;;

            const float sqrtd = std::sqrtf(disc);
            float t0 = (h - sqrtd) * inv_a;
            float t1 = (h + sqrtd) * inv_a;

            bool second = (t0 < r.t_min) || (t0 >= r.t_max);
            float thit = second ? t1 : t0;

            bool notHit = (thit < r.t_min) || (thit >= r.t_max);
            if (notHit) continue;;

            r.t_max = thit;
            hitIdx = i;

        }

        if (hitIdx < 0) {
            return attenuation * background(r);
        }
        //rec.p = r.at(rec.t);
        r.orig = r.at(r.t_max);
        r.normal = r.orig * invGeom[hitIdx].inv_radius - invGeom[hitIdx].v;


        color c;
        if (!mats[hitIdx].scatter(r, c)) {
            return color(0,0,0);
        }

        attenuation = attenuation * c;
    }

    return color(0,0,0); // exceeded bounce limit
}

#endif // !SCENE_H

