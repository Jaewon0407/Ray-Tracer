#pragma once
#include "ray.h"
#include "interval.h"

struct hit_record {
    Vec3 p;
    Vec3 normal;
    double t;
    bool front_face;

    void set_face_normal(const Ray& r, const Vec3& outside_normal) {
        // ray is pointing towards the opposite direction of normal
        if (r.direction.dot(outside_normal) < 0.0) {
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