#pragma once

#include <fstream>
#include <iostream>
#include <vector>

#include "hittable.h"
#include "material.h"
#include "ray.h"

class Camera {
public:

    double aspect_ratio = 16.0 / 9.0;
    int image_width = 400;
    int samples_per_pixel = 4;
    int max_depth = 10; // maximum number of ray bounces into the scene

    double vfov = 90; // vertical view angle
    Vec3 lookFrom = Vec3(0,0,0);
    Vec3 lookAt = Vec3(0,0,-1);
    Vec3 vUp = Vec3 (0,1,0);

    double defocus_angle = 0; // variation angle of rays through each pixel
    double focus_dist = 10; // distance from camera lookFrom point to plane of perfect focus

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

                    auto ray_origin = (defocus_angle <= 0) ? camera_center : defocus_disk_sample();
                    auto ray_direction = pixel_sample - ray_origin;

                    Ray r(ray_origin, ray_direction);
                    pixel_color = pixel_color + ray_color(r, world, max_depth);
                }

                static const Interval intensity(0.000, 0.999);
                
                // image viewier is expecting an image in gamma space, but 
                // the image is given in linear space
                auto r = linear_to_gamma(pixel_color.x / samples_per_pixel);
                auto g = linear_to_gamma(pixel_color.y / samples_per_pixel);
                auto b = linear_to_gamma(pixel_color.z / samples_per_pixel);

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
    Vec3 u, v, w;

    Vec3 defocus_disk_u;
    Vec3 defocus_disk_v;

    void initialize() {
        image_height = int(image_width/aspect_ratio);

        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta/2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (double(image_width)/image_height);
        camera_center = lookFrom;

        w = (lookFrom - lookAt).normalize(); // view direction
        u = vUp.crossProduct(w).normalize(); // width
        v = w.crossProduct(u); // height - automatically normalized

        // defines how the viewport is laid out in the 3D space
        auto viewport_u = viewport_width * u;
        auto viewport_v = viewport_height * -v;

        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        auto viewport_upper_left = camera_center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + pixel_delta_u/2 + pixel_delta_v/2;

        auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    Vec3 defocus_disk_sample() const {
        auto p = random_in_unit_disk();
        return camera_center + (p.x * defocus_disk_u) + (p.y * defocus_disk_v);
    }

    Vec3 ray_color(const Ray& r, const Hittable& world, int depth) const {
        
        if (depth <= 0) {
            return Vec3(0,0,0);
        }
        
        hit_record rec;
        if (world.hit(r, Interval(0.001, infinity), rec)) {
            Ray scattered;
            Vec3 attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered)) {
                // attenuation represents how much light is absorbed
                return attenuation * ray_color(scattered, world, depth-1);
            }
            // recursively cast rays
            return Vec3(0,0,0); 
        }

        // interpolate between white and light blue
        // rays point up are more blue; rays point down are more white
        Vec3 unit_direction = r.direction.normalize();
        auto a = 0.5 * (unit_direction.y + 1.0);
        return (1.0 - a) * Vec3(1.0, 1.0, 1.0) + a * Vec3(0.5, 0.7, 1.0);
    }
};