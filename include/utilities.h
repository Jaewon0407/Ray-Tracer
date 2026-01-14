#pragma once

#include <limits>
#include <random>

// #include "vec3.h"

constexpr double infinity = std::numeric_limits<double>::infinity();
constexpr double pi = 3.1415926535897932385;
constexpr double epsilon = 1e-8;

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double linear_to_gamma(double linear_component) {
    if (linear_component > 0) {
        return std::sqrt(linear_component);
    }
    return 0;
}

inline double random_offset() {
    static std::uniform_real_distribution<double> distribution(-0.5, 0.5);
    static std::mt19937 generator;
    return distribution(generator);
}

// inline double random_double() {
//     static thread_local std::mt19937 generator(std::random_device{}());
//     static thread_local std::uniform_real_distribution<double> distribution(0.0, 1.0);
//     return distribution(generator);
// }

inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}