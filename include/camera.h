#pragma once

#include <fstream>
#include <iostream>
#include <vector>

#include "hittable.h"
#include "ray.h"

class Camera {
public:

    double aspect_ratio = 16.0 / 9.0;
    int image_width = 400;
    int samples_per_pixel = 4;
    int max_depth = 10; // maximum number of ray bounces into the scene

    void render(const Hittable& world) {

        initialize();

        std::ofstream out("../data/output.ppm");

        out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++) {
            for (int i = 0; i < image_width; i++) {
                
                Vec3 pixel_color(0,0,0);

                // for MSAA, we want to cast multiple rays 
                for (int sample = 0; sample < samples_per_pixel; sample++) {

                    // set offset
                    auto offset_u = random_offset() * pixel_delta_u;
                    auto offset_v = random_offset() * pixel_delta_v;

                    auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
                    auto pixel_sample = pixel_center + offset_u + offset_v;
                    auto ray_direction = pixel_sample - camera_center;

                    Ray r(camera_center, ray_direction);
                    pixel_color = pixel_color + ray_color(r, world, max_depth);

                    // std::cout << "pixel_color: (" << pixel_color.x << ", " << pixel_color.y << ", " << pixel_color.z << ")" << std::endl;
                }

                static const Interval intensity(0.000, 0.999);

                auto r = pixel_color.x / samples_per_pixel;
                auto g = pixel_color.y / samples_per_pixel;
                auto b = pixel_color.z / samples_per_pixel;

                int ir = static_cast<int>(256 * intensity.clamp(r));
                int ig = static_cast<int>(256 * intensity.clamp(g));
                int ib = static_cast<int>(256 * intensity.clamp(b));

                out << ir << ' ' << ig << ' ' << ib << "\n";
            }
        }

        out.close();
    }

private:

    int image_height;
    Vec3 camera_center;
    Vec3 pixel00_loc;
    Vec3 pixel_delta_u;
    Vec3 pixel_delta_v;

    void initialize() {
        image_height = int(image_width/aspect_ratio);

        auto focal_length = 1.0;
        auto viewport_height = 2.0;
        auto viewport_width = viewport_height * (double(image_width)/image_height);
        camera_center = Vec3(0,0,0);

        // defines how the viewport is laid out in the 3D space
        auto viewport_u = Vec3(viewport_width, 0, 0);
        auto viewport_v = Vec3(0, -viewport_height, 0);

        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // std::cout << "pixel_delta_u: (" << pixel_delta_u.x << ", " << pixel_delta_u.y << ", " << pixel_delta_u.z << ")" << std::endl;

        // find the location of the upper left pixel
        auto viewport_center = camera_center;
        auto viewport_upper_left = viewport_center - Vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + pixel_delta_u/2 + pixel_delta_v/2;
    }

    Vec3 ray_color(const Ray& r, const Hittable& world, int depth) const {
        
        if (depth <= 0) {
            return Vec3(0,0,0);
        }
        
        hit_record rec;
        if (world.hit(r, Interval(0.001, infinity), rec)) {
            Vec3 direction = rec.normal + random_unit_vector();
            // recursively cast rays
            return 0.5 * ray_color(Ray(rec.p, direction), world, depth-1);    
        }

        // interpolate between white and light blue
        // rays point up are more blue; rays point down are more white
        Vec3 unit_direction = r.direction.normalize();
        auto a = 0.5 * (unit_direction.y + 1.0);
        return (1.0 - a) * Vec3(1.0, 1.0, 1.0) + a * Vec3(0.5, 0.7, 1.0);
    }
};