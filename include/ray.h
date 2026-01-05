#pragma once

#include "vec3.h"

class Ray {
public:
    Vec3 origin;
    Vec3 direction;

    Ray() : origin(), direction() {}

    Ray(Vec3 origin_, Vec3 direction_) : origin(origin_), direction(direction_) {}

    Vec3 at(double t) const {
        return origin + t * direction;
    }
};
