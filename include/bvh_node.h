#pragma once
#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"

class bvh_node : public Hittable {
public:
    std::shared_ptr<Hittable> left;
    std::shared_ptr<Hittable> right;
    aabb bbox;

    bvh_node(const std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end) {
        // take a list of objects and a range (start, end)
        // if only one object, this node is a leaf
        auto object_span = end - start;
        if (object_span == 1) {
            // store the object as a child
            left = right = objects[start];
        } else if (object_span == 2) {
            left = objects[start];
            right = objects[start+1];
        } else {

        }
    }
};