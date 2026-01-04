
#pragma once
#include "utilities.h"

class Interval {
public:
    double min, max;

    Interval() : min(infinity), max(-infinity) {}
    Interval(double min_, double max_) : min(min_), max(max_) {}

    double size() const {
        return max - min;
    }

    bool contains(double x) const {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const {
        return min < x && x < max;
    }

    double clamp(double x) const {
        if (x < min) { return min; }
        if (x > max) { return max; }
        return x;
    }
};