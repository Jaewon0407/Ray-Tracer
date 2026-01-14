#pragma once

#include <memory>
#include <vector>

#include "hittable.h"
#include "interval.h"

class Hittable_list : public Hittable {
public:
    std::vector<std::shared_ptr<Hittable>> objects;

    Hittable_list() {}

    void clear() { objects.clear(); }
    void add(std::shared_ptr<Hittable> object) {
        objects.push_back(object);
        bbox = aabb(bbox, object->bounding_box());
    }

    bool hit(const Ray& r, Interval ray_t, hit_record& rec) const override {
        // want to iterate objects and check for ray intersection
        hit_record temp_rec;
        auto closest_so_far = ray_t.max;
        bool hit_anything = false;
        
        // ray_t.max and ray_t.min reset to the original interval, in the subsequent ray cast
        // this is for one single ray
        for (const auto& object: objects) {
            if (object->hit(r, Interval(ray_t.min, closest_so_far), temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }
        return hit_anything;
    }
private:
    aabb bbox;
};