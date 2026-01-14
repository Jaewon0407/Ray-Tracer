#pragma once

#include "utilities.h"

class Interval {
public:
    double min, max;

    Interval() : min(infinity), max(-infinity) {}
    Interval(double min_, double max_) : min(min_), max(max_) {}

    // This combines bounding intervals of multiple objects
    Interval(const Interval& a, const Interval& b) {
        // Create the interval tightly enclosing the 2 input boxs' intervals
        if (a.min <= b.min) {
            min = a.min;
        } else {
            min = b.min;
        }

        if (a.max >= b.max) {
            max = a.max;
        } else {
            max = b.max;
        }

    }

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