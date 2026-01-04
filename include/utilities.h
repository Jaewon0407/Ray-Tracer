#pragma once
#include <limits>
#include <random>

constexpr double infinity = std::numeric_limits<double>::infinity();
constexpr double pi = 3.1415926535897932385;
constexpr double epsilon = 1e-8;

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double random_offset() {
    static std::uniform_real_distribution<double> distribution(-0.5, 0.5);
    static std::mt19937 generator;
    return distribution(generator);
}