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

#include "rtweekend.h"

#include "camera.h"
#include "material.h"
#include "scene.h"
#include <vector>


std::vector<material> materials;
std::vector<material>& getMaterials() {return materials;}
Spheres spheresAOS;
Spheres& getSpheres() {return spheresAOS;}



int addMaterial(const material & mat) {
    materials.push_back(mat);
    return materials.size() - 1;

}

int main() {
    Timer t("raytracer");
    //spheres.push_back(sphere{point3(0,-1000,0), 1000, ground_material});
    //
    int count = 20;
    spheresAOS.geom.reserve((11*2)*(11*2) + 4);
    spheresAOS.invGeom.reserve((11*2)*(11*2) + 4);
    spheresAOS.add(point3(0,-1000,0), 1000);
    addMaterial(material::make_lambert(color(0.5, 0.5, 0.5)));



    for (int a = -count; a < count; a++) {
        for (int b = -count; b < count; b++) {
            auto choose_mat = random_float();
            point3 center(a + 0.9*random_float(), 0.2, b + 0.9*random_float());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = vec3_random() * vec3_random();
                    addMaterial(material::make_lambert(albedo));

                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = vec3_random(0.5, 1);
                    auto fuzz = random_float(0, 0.5);
                    addMaterial(material::make_metal(albedo, fuzz));
                    //spheres.push_back(sphere{center, 0.2, mat});
                } else {
                    addMaterial(material::make_dielectric(1.5));
                }
                spheresAOS.add(center, 0.2);
            }
        }
    }

    spheresAOS.add(point3(0, 1, 0), 1.0);
    spheresAOS.add(point3(-4, 1, 0), 1.0);
    spheresAOS.add(point3(4, 1, 0), 1.0);
    addMaterial(material::make_dielectric(1.5));
    addMaterial(material::make_lambert(color(0.4, 0.2, 0.1)));
    addMaterial(material::make_metal(color(0.7, 0.6, 0.5), 0.0));
    /*spheres.push_back(sphere{point3(0, 1, 0), 1.0, glassMat});
    spheres.push_back(sphere{point3(-4, 1, 0), 1.0, addMaterial(material::make_lambert(color(0.4, 0.2, 0.1)))});
    spheres.push_back(sphere{point3(4, 1, 0), 1.0, addMaterial(material::make_metal(color(0.7, 0.6, 0.5), 0.0))});*/


    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 1200;
    cam.samples_per_pixel = 50;

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    Timer t2("rendering");
    cam.render();
}
