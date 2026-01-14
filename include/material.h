#pragma once

#include <iostream>

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

    bool scatter(const Ray& r_in, const hit_record& rec, Vec3& attenuation, Ray& scattered) const override {
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

    bool scatter(const Ray& r_in, const hit_record& rec, Vec3& attenuation, Ray& scattered) const override {
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

// refractive index - amount ray bends
// relative refractive index = refractive index of the object's material / refractive index of other materials
class Dielectric : public Material {
public: 
    Dielectric(double refraction_index_) : refraction_index(refraction_index_) {}

    bool scatter(const Ray& r_in, const hit_record& rec, Vec3& attenuation, Ray& scattered) const override {

        attenuation = Vec3(1.0, 1.0, 1.0);

        // ray is entering the material
        double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;

        Vec3 r_in_normalized = r_in.direction.normalize();
        double cos_theta = std::fmin(-r_in_normalized.dot(rec.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
        bool cannot_refract = ri * sin_theta > 1.0;
        
        Vec3 direction;
        if (cannot_refract || reflectance(cos_theta, ri) > random_double()) {
            direction = reflect(r_in_normalized, rec.normal); // if cannot refract
        } else {
            direction = refract(r_in_normalized, rec.normal, ri); 
        }
    
        scattered = Ray(rec.p, direction);
        return true;
    };

private:
    double refraction_index;

    static double reflectance(double cosine, double refraction_index) {
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0*r0;
        return r0 + (1-r0) * std::pow((1-cosine), 5);
    }
};