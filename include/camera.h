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

    void render(const Hittable& world) {

        initialize();

        std::ofstream out("../data/output.ppm");
        out << "P3\n" << image_width << " " << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++) {
            for (int i = 0; i < image_width; i++) {
                auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
                auto ray_direction = pixel_center - camera_center;
                Ray r(camera_center, ray_direction);

                Vec3 color = ray_color(r, world);
                
                int ir = static_cast<int>(255.999 * color.x);
                int ig = static_cast<int>(255.999 * color.y);
                int ib = static_cast<int>(255.999 * color.z);

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

        // find the location of the upper left pixel
        auto viewport_center = camera_center;
        auto viewport_upper_left = viewport_center - Vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + pixel_delta_u/2 + pixel_delta_v/2;
    }

    Vec3 ray_color(const Ray& r, const Hittable& world) const {
        hit_record rec;
        if (world.hit(r, Interval(0, infinity), rec)) {
            return 0.5 * Vec3(rec.normal.x + 1, rec.normal.y + 1, rec.normal.z + 1);    
        }

        // interpolate between white and light blue
        // rays point up are more blue; rays point down are more white
        Vec3 unit_direction = r.direction.normalize();
        auto a = 0.5 * (unit_direction.y + 1.0);
        return (1.0 - a) * Vec3(1.0, 1.0, 1.0) + a * Vec3(0.5, 0.7, 1.0);
    }
};