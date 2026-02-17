#ifndef RTWEEKEND_H
#define RTWEEKEND_H
//==============================================================================================
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi
using vec3 = glm::vec3;
using point3 = glm::vec3;
using color = vec3;
using namespace glm;

// Constants

const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385;


// Utility Functions

inline float degrees_to_radians(float degrees) {
    return degrees * M_PI / 180.0;
}


struct Timer {
    Timer(const char* name);
    ~Timer();
    const char *name;
    float duration;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
};


Timer::Timer(const char* name) : name(name), start(std::chrono::high_resolution_clock::now()) {
    std::clog << "\rTimer " << name << " started\n";
}

Timer::~Timer() {
    auto end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration<float>(end - start).count();

    std::clog << "\rTimer " << name << " finished, took: " << duration << "s\n";
}


using u32 = unsigned int;
inline float fastRandom(u32 & seed) {
    //using PCG HASH
    u32 state = seed * 747796405u + 2891336453u;
    u32 word = ((state >> ((state >> 28u) + 4u))^state) * 277803737u;
    seed = (word >> 22u) ^ word;
    float ret = (float)seed / (float)UINT32_MAX; 
    assert(ret <= 1.0f);
    assert(ret >= 0.0f);
    return ret;
}


//----------------- RANDOM ---------------//
thread_local u32 seed = 213123;
inline float random_float() {
    return fastRandom(seed);
    // Returns a random real in [0,1).
    return std::rand() / (RAND_MAX + 1.0);
}

inline float random_float(float min, float max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_float();
}
static vec3 vec3_random() {
    return vec3(random_float(), random_float(), random_float());
}

static vec3 vec3_random(float min, float max) {
    return vec3(random_float(min,max), random_float(min,max), random_float(min,max));
}

inline vec3 unit_vector(const vec3& v) {
    return normalize(v);
}

inline vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_float(-1,1), random_float(-1,1), 0);
        if (dot(p, p) < 1)
            return p;
    }
}

inline vec3 random_unit_vector() {
    while (true) {
        auto p = vec3_random(-1,1);
        auto lensq = dot(p, p);
        if (1e-160 < lensq && lensq <= 1.0)
            return p / sqrt(lensq);
    }
}

inline vec3 random_on_hemisphere(const vec3& normal) {
    vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}




#endif
