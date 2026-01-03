#pragma once
#include <cmath>

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
    Vec3 operator*(double s) const {
        return Vec3(x * s, y * s, z * s);
    }
    Vec3 operator/(double s) const {
        return Vec3(x / s, y / s, z / s);
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
};

inline Vec3 operator*(double s, const Vec3& v) {
    return Vec3(v.x * s, v.y * s, v.z * s);
}