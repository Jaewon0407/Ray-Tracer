#pragma once

#include "hittable.h"
#include "ray.h"
#include "utilities.h"
#include "vec3.h"

class Material {
public:
    virtual ~Material() = default;

    virtual bool scatter(
        const Ray& r_in, const hit_record& rec, Vec3& attenuation, Ray& scattered
    ) const {
        return false;
    }
};

// surfaces need to scatter light randomly to look matte/diffuse
class Lambertian : public Material {
public:
    Lambertian(const Vec3& albedo_) : albedo(albedo_) {}

    bool scatter(const Ray& r_in, const hit_record& rec, Vec3& attenuation, Ray& scattered) const {
        auto scatter_direction = random_unit_vector() + rec.normal;
        // opt out length for length_squared, since length is too expensive
        if (scatter_direction.length_squared() < 1e-8) {
            scatter_direction = rec.normal;
        }
        scattered = Ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

private:
    Vec3 albedo;
};

class Metal : public Material {
public: 
    Metal(const Vec3& albedo_, double fuzz_) : albedo(albedo_), fuzz(fuzz_) {}

    bool scatter(const Ray& r_in, const hit_record& rec, Vec3& attenuation, Ray& scattered) const {
        auto reflect_direction = reflect(r_in.direction, rec.normal);
        reflect_direction = reflect_direction.normalize() + (fuzz * random_unit_vector());
        scattered = Ray(rec.p, reflect_direction);
        attenuation = albedo;
        return (scattered.direction.dot(rec.normal) > 0);
    };

private:
    Vec3 albedo;
    double fuzz;
};