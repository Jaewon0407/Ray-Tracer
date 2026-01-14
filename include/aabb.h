#pragma once

#include "vec3.h"
#include "interval.h"
#include "ray.h"

class aabb {
public:
    Interval x, y, z;

    aabb() {}
    aabb(const Interval& x_, const Interval& y_, const Interval& z_) : x(x_), y(y_), z(z_) {}
    aabb(const Vec3& v1, const Vec3& v2) {
        if (v1.x > v2.x) {
            x = Interval(v1.x, v2.x);
        } else {
            x = Interval(v2.x, v1.x);
        }

        if (v1.y > v2.y) {
            y = Interval(v1.y, v2.y);
        } else {
            y = Interval(v2.y, v1.y);
        }

        if (v1.z > v2.z) {
            z = Interval(v1.z, v2.z);
        } else {
            z = Interval(v2.z, v1.z);
        }
    }
    aabb(const aabb& box0, const aabb& box1) {
        x = Interval(box0.x, box1.x);
        y = Interval(box0.y, box1.y);
        z = Interval(box0.z, box1.z);
    }

    const Interval& axis_interval(int n) const {
        if (n == 0) {
            return x;
        } else if (n == 1) {
            return y;
        } else {
            return z;
        }
    }

    bool hit(const Ray& r, Interval ray_t) const {

        const Vec3& ray_origin = r.origin;
        const Vec3& ray_direction = r.direction;

        // check for intersection for each axis
        for (int i = 0; i < 3; i++) {
            // x, y, z
            const Interval& axis = axis_interval(i);

            auto t0 = (axis.min - ray_origin[i]) / ray_direction[i]; 
            auto t1 = (axis.max - ray_origin[i]) / ray_direction[i];
            
            // if the t0 is the entry and the t1 is the exit
            if (t0 < t1) {
                if (t0 > ray_t.min) ray_t.min = t0;
                if (t1 < ray_t.max) ray_t.max = t1;
            } else {
                if (t1 > ray_t.min) ray_t.min = t1;
                if (t0 > ray_t.max) ray_t.max = t0;
            }

            if (ray_t.max <= ray_t.min)
                return false;
        }
        return true;
    }
};