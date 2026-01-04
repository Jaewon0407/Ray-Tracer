#include "../include/vec3.h"
#include "../include/ray.h"
#include "../include/sphere.h"
#include "../include/hittable_list.h"
#include "../include/interval.h"
#include "../include/camera.h"
#include "../include/utilities.h"

int main() {
    
    Camera camera;

    camera.aspect_ratio = 16.0/9.0;
    camera.image_width = 400;
    camera.samples_per_pixel = 100;

    Hittable_list world;
    world.add(std::make_shared<Sphere>(Vec3(0,0,-1), 0.5));
    world.add(std::make_shared<Sphere>(Vec3(0,-100.5,-1), 100));

    camera.render(world);
    return 0;   
}