#include "../include/vec3.h"
#include "../include/ray.h"

#include <fstream>
#include <iostream>

std::pair<double, double> intersect_sphere(const Ray& r, const Vec3& center, double radius) {
    double a = r.direction.dot(r.direction);
    double h = r.direction.dot(center - r.origin);
    double c = (center - r.origin).dot(center - r.origin) - radius*radius;
    double discriminant = h*h - a*c;
    if (discriminant < 0.0) {
        return {-1, -1};
    } else {
        // if there is one solution, both t1 and t2 will be equal
        double t1 = (h - std::sqrt(discriminant)) / a;
        double t2 = (h + std::sqrt(discriminant)) / a;
        return {t1, t2};
    }
}

Vec3 ray_color(const Ray& r, const Vec3& sphere_center, double sphere_radius) {
    auto roots = intersect_sphere(r, sphere_center, sphere_radius);
    // get the closest intersection for now
    auto t = std::min(roots.first, roots.second);

    if (t > 0.0) {
        auto surface_normal = Vec3(r.at(t) - sphere_center).normalize();
        return 0.5 * Vec3(surface_normal.x + 1, surface_normal.y + 1, surface_normal.z + 1);    
    }

    // interpolate between white and light blue
    // rays point up are more blue; rays point down are more white
    Vec3 unit_direction = r.direction.normalize();
    auto a = 0.5 * (unit_direction.y + 1.0);
    return (1.0 - a) * Vec3(1.0, 1.0, 1.0) + a * Vec3(0.5, 0.7, 1.0);
}

int main() {

    double aspect_ratio = 16.0 / 9.0;
    int image_width = 400;
    int image_height = int(image_width/aspect_ratio);

    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (double(image_width)/image_height);
    auto camera_center = Vec3(0,0,0);

    // defines how the viewport is laid out in the 3D space
    auto viewport_u = Vec3(viewport_width, 0, 0);
    auto viewport_v = Vec3(0, -viewport_height, 0);

    auto pixel_delta_u = viewport_u / image_width;
    auto pixel_delta_v = viewport_v / image_height;

    // find the location of the upper left pixel
    auto viewport_center = camera_center;
    auto viewport_upper_left = viewport_center - Vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
    auto pixel00_loc = viewport_upper_left + pixel_delta_u/2 + pixel_delta_v/2;
    
    // open output file
    std::ofstream out("../data/output.ppm");
    out << "P3\n" << image_width << " " << image_height << "\n255\n";

    Vec3 sphere_center(0,0,-1);
    double sphere_radius = 0.5;

    // render
    for (int j = 0; j < image_height; j++) {
        for (int i = 0; i < image_width; i++) {
            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            Ray r(camera_center, ray_direction);

            // std::cout << "surface_normal: (" << surface_normal.x << ", " << surface_normal.y << ", " << surface_normal.z << ")" << std::endl; 
            Vec3 color = ray_color(r, sphere_center, sphere_radius);
            
            int ir = static_cast<int>(255.999 * color.x);
            int ig = static_cast<int>(255.999 * color.y);
            int ib = static_cast<int>(255.999 * color.z);

            out << ir << ' ' << ig << ' ' << ib << "\n";
        }
    }

    out.close();
    return 0;   
}