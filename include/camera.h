#pragma once

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <thread>
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

    bool fisheye = false;
    double fisheye_fov = 180;

    int num_threads = 0;

    void render(const Hittable& world) {

        initialize();

        std::vector<Vec3> framebuffer(image_width * image_height);

        int thread_count = num_threads > 0 ? num_threads
                                           : static_cast<int>(std::thread::hardware_concurrency());
        thread_count = std::max(1, thread_count);

        std::atomic<int> next_row{0};
        std::atomic<int> rows_done{0};

        auto worker = [&]() {
            for (int j = next_row++; j < image_height; j = next_row++) {
                for (int i = 0; i < image_width; i++) {
                    framebuffer[j * image_width + i] = render_pixel(i, j, world);
                }
                int done = ++rows_done;
                if (done % 20 == 0 || done == image_height) {
                    std::clog << "\rRows remaining: " << (image_height - done) << ' ' << std::flush;
                }
            }
        };

        auto start = std::chrono::steady_clock::now();

        std::vector<std::thread> threads;
        for (int t = 0; t < thread_count; t++) {
            threads.emplace_back(worker);
        }
        for (auto& thread : threads) {
            thread.join();
        }

        auto seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
        std::clog << "\rRendered with " << thread_count << " threads in " << seconds << " s\n";

        std::ofstream out("../data/output.ppm");
        out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        static const Interval intensity(0.000, 0.999);

        for (const auto& pixel_color : framebuffer) {
            auto r = linear_to_gamma(pixel_color.x / samples_per_pixel);
            auto g = linear_to_gamma(pixel_color.y / samples_per_pixel);
            auto b = linear_to_gamma(pixel_color.z / samples_per_pixel);

            int ir = static_cast<int>(256 * intensity.clamp(r));
            int ig = static_cast<int>(256 * intensity.clamp(g));
            int ib = static_cast<int>(256 * intensity.clamp(b));

            out << ir << ' ' << ig << ' ' << ib << "\n";
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

    Vec3 render_pixel(int i, int j, const Hittable& world) const {
        Vec3 pixel_color(0,0,0);

        for (int sample = 0; sample < samples_per_pixel; sample++) {

            if (fisheye) {
                Vec3 dir;
                if (fisheye_direction(i + 0.5 + random_offset(), j + 0.5 + random_offset(), dir)) {
                    pixel_color = pixel_color + ray_color(Ray(camera_center, dir), world, max_depth);
                }
                continue;
            }

            auto offset_u = random_offset() * pixel_delta_u;
            auto offset_v = random_offset() * pixel_delta_v;

            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto pixel_sample = pixel_center + offset_u + offset_v;

            auto ray_origin = (defocus_angle <= 0) ? camera_center : defocus_disk_sample();
            auto ray_direction = pixel_sample - ray_origin;

            Ray r(ray_origin, ray_direction);
            pixel_color = pixel_color + ray_color(r, world, max_depth);
        }
        return pixel_color;
    }

    bool fisheye_direction(double px, double py, Vec3& dir) const {
        double radius_px = std::min(image_width, image_height) / 2.0;

        double x = (px - image_width / 2.0) / radius_px;
        double y = (py - image_height / 2.0) / radius_px;

        double r = std::sqrt(x * x + y * y);
        if (r > 1.0) {
            return false;
        }

        double theta = r * degrees_to_radians(fisheye_fov) / 2.0;
        double phi = std::atan2(y, x);

        dir = std::sin(theta) * std::cos(phi) * u
            + std::sin(theta) * std::sin(phi) * -v
            + std::cos(theta) * -w;
        return true;
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