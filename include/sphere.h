#pragma once

#include "hittable.h"
#include "aabb.h"
#include "vec3.h"

class Sphere : public Hittable {
public:
    Sphere(const Vec3& center_, double radius_, std::shared_ptr<Material> mat_) : center(center_), radius(radius_), mat(mat_) {
        bbox = aabb(Vec3(center.x - radius, center.y - radius, center.z - radius),
                    Vec3(center.x + radius, center.y + radius, center.z + radius));
    }

    bool hit(const Ray& r, Interval ray_t, hit_record& rec) const override {
        
        double a = r.direction.dot(r.direction);
        double h = r.direction.dot(center - r.origin);
        double c = (center - r.origin).dot(center - r.origin) - radius*radius;
        double discriminant = h*h - a*c;
        
        if (discriminant < 0.0) {
            return false;
        } else {
            double sqrtd = std::sqrt(discriminant);
            
            // find the nearest root in the acceptable range
            // ray_tmax = infinity initially
            double root = (h - sqrtd) / a;
            if (!ray_t.surrounds(root)) {
                root = (h + sqrtd) / a;
                if (!ray_t.surrounds(root)) {
                    return false;
                }
            }

            rec.t = root;
            rec.mat = mat;
            rec.p = r.at(rec.t);
            Vec3 outside_normal = (rec.p - center).normalize();
            rec.set_face_normal(r, outside_normal);

            return true;
        }
    }

    aabb bounding_box() const override { return bbox; }

private:
    Vec3 center;
    double radius;
    std::shared_ptr<Material> mat;
    aabb bbox;
};