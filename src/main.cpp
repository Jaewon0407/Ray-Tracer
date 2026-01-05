#include "../include/camera.h"
#include "../include/hittable_list.h"
#include "../include/interval.h"
#include "../include/ray.h"
#include "../include/sphere.h"
#include "../include/utilities.h"
#include "../include/vec3.h"
#include "../include/material.h"

int main() {
    
    Camera camera;

    camera.aspect_ratio = 16.0/9.0;
    camera.image_width = 400;
    camera.samples_per_pixel = 100;
    camera.max_depth = 50;

    Hittable_list world;
    auto material_ground = std::make_shared<Lambertian>(Vec3(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<Lambertian>(Vec3(0.1, 0.2, 0.5));
    auto material_left   = std::make_shared<Metal>(Vec3(0.8, 0.8, 0.8), 0.1);
    auto material_right  = std::make_shared<Metal>(Vec3(0.8, 0.6, 0.2), 0.1);

    world.add(std::make_shared<Sphere>(Vec3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(std::make_shared<Sphere>(Vec3( 0.0,    0.0, -1.2),   0.5, material_center));
    world.add(std::make_shared<Sphere>(Vec3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(std::make_shared<Sphere>(Vec3( 1.0,    0.0, -1.0),   0.5, material_right));

    camera.render(world);
    return 0;   
}