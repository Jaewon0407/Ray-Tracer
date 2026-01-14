#pragma once

#include <cmath>

#include "utilities.h"

class Vec3 {
public:
    double x, y, z;

    Vec3() : x(0), y(0), z(0) {}

    Vec3(double a, double b, double c) : x(a), y(b), z(c) {}

    Vec3 operator+(const Vec3& v) const {
        return Vec3(x + v.x, y + v.y, z + v.z);
    }
    Vec3 operator-(const Vec3& v) const {
        return Vec3(x - v.x, y - v.y, z - v.z);
    }
    Vec3 operator-() const {
        return Vec3(-x, -y, -z);
    }
    Vec3 operator*(const Vec3& v) const {
        return Vec3(x * v.x, y * v.y, z * v.z);
    }
    Vec3 operator*(double s) const {
        return Vec3(x * s, y * s, z * s);
    }
    Vec3 operator/(double s) const {
        return Vec3(x / s, y / s, z / s);
    }

    double operator[](int i) const {
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }

    double dot(const Vec3& v) const {
        return x * v.x + y * v.y + z * v.z;
    }
    Vec3 crossProduct(const Vec3& v) const {
        return Vec3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }
    double length() const {
        return std::sqrt(x * x + y * y + z * z);
    }
    Vec3 normalize() const {
        double len = length();
        return Vec3(x / len, y / len, z / len);
    }

    double length_squared() const {
        return (x*x + y*y + z*z);
    }

    static Vec3 random() {
        return Vec3(random_double(), random_double(), random_double());
    }

    static Vec3 random(double min, double max) {
        return Vec3(random_double(min, max), random_double(min, max), random_double(min, max));
    }

};

inline Vec3 operator*(double s, const Vec3& v) {
    return Vec3(v.x * s, v.y * s, v.z * s);
}

inline Vec3 operator+(const Vec3& v, double s) {
    return Vec3(v.x + s, v.y + s, v.z + s);
}

inline Vec3 operator-(const Vec3& v, double s) {
    return Vec3(v.x - s, v.y - s, v.z - s);
}

inline Vec3 random_in_unit_disk() {
    while (true) {
        auto p = Vec3(random_double(-1,1), random_double(-1,1), 0);
        // if p is within unit sphere, return p
        if (p.length_squared() < 1) {
            return p;
        }
    }
}

// Pick random point in the unit sphere and generate a normalized vector
inline Vec3 random_unit_vector() {
    while (true) {
        auto p = Vec3(random_double(), random_double(), random_double());
        auto lensq = p.length_squared();
        if (1e-160 < lensq && lensq <= 1) {
            return p / sqrt(lensq);
        }
    }
}

inline Vec3 reflect(const Vec3& v, const Vec3& normal) {
    return v - 2 * v.dot(normal) * normal;
}

inline Vec3 refract(const Vec3& v, const Vec3& normal, double etai_over_etat) {
    // std::fmin returns min of 2 floating point numbers
    auto cos_theta = std::fmin(-v.dot(normal), 1.0);
    Vec3 r_perp = etai_over_etat * (v + cos_theta * normal);
    Vec3 r_parallel = -std::sqrt(std::fabs(1.0 - r_perp.length_squared())) * normal;
    return r_perp + r_parallel;
}