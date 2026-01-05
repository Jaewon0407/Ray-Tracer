#pragma once

#include <memory>

#include "interval.h"
#include "ray.h"

class Material;

struct hit_record {
    Vec3 p;
    Vec3 normal;
    double t;
    bool front_face;
    std::shared_ptr<Material> mat;

    void set_face_normal(const Ray& r, const Vec3& outside_normal) {
        // ray is pointing towards the opposite direction of normal
        front_face = r.direction.dot(outside_normal) < 0.0;
        if (front_face) {
            normal = outside_normal;
        } else {
            normal = -outside_normal;
        }
    }
};

class Hittable {
public:
    virtual ~Hittable() = default;
    virtual bool hit(const Ray& r, Interval ray_t, hit_record& rec) const = 0;
};