#pragma once
#include "object.h"
#include "vec.h"
#include "material.h"
#include "transform.h"
#include <cmath>

#define M_PI 3.14159265358979323846

class Sphere : public Object {
public:
    Sphere( const Vec& c, float r ) : center(c), radius(r) {}

    bool intersect( const Ray& ray, Hit& hit ) const override {
        // Transform ray to object space
        Ray local_ray = transform.inverse_transform_ray( ray );

        // Calculate quadratic equation coefficients
        Vec oc = local_ray.origin - center;
        float a = Vec::dot( local_ray.direction, local_ray.direction );
        float b = 2.0f * Vec::dot( oc, local_ray.direction );
        float c = Vec::dot( oc, oc ) - radius * radius;

        // Calculate discriminant
        float discriminant = b * b - 4.0f * a * c;
        if ( discriminant < 0 ) {
            return false;
        }

        // Calculate intersection point
        float local_t = ( -b - std::sqrt( discriminant ) ) / ( 2.0f * a );
        if ( local_t < ray.min_t || local_t > ray.max_t ) {
            local_t = ( -b + std::sqrt( discriminant ) ) / ( 2.0f * a );
            if ( local_t < ray.min_t || local_t > ray.max_t ) {
                return false;
            }
        }

        // Calculate hit point and normal in local space
        Vec local_point = local_ray.origin + local_ray.direction * local_t;
        Vec local_normal = ( local_point - center ).normalize();

        // Transform back to world space
        Vec world_point = transform.transform_point( local_point );
        Vec world_normal = transform.transform_normal( local_normal ).normalize();
        
        // Recalculate t for world space ray
        Vec to_hit = world_point - ray.origin;
        float world_t = Vec::dot( to_hit, ray.direction.normalize() );
        
        if ( world_t < ray.min_t || world_t > ray.max_t ) {
            return false;
        }
        
        hit.t = world_t;
        hit.point = world_point;
        hit.normal = world_normal;
        hit.material = material;

        // Calculate UV coordinates
        float phi = std::atan2( local_normal.z, local_normal.x );
        float theta = std::asin( local_normal.y );
        hit.u = ( phi + M_PI ) / ( 2.0f * M_PI );
        hit.v = ( theta + M_PI / 2.0f ) / M_PI;

        return true;
    }

    Vec get_normal( const Vec& point ) const override {
        Vec local_point = transform.inverse_transform_point( point );
        Vec local_normal = ( local_point - center ).normalize();
        return transform.transform_normal( local_normal );
    }

    Vec2 get_uv( const Vec& point ) const {
        Vec local_point = transform.inverse_transform_point( point );
        Vec local_center = transform.inverse_transform_point( center );
        Vec local_dir = ( local_point - local_center ).normalize();
        float u = 0.5f + std::atan2( local_dir.z, local_dir.x ) / ( 2.0f * M_PI );
        float v = 0.5f - std::asin( local_dir.y ) / M_PI;
        return Vec2( u, v );
    }

    void set_center( const Vec& new_center ) { center = new_center; }
    Vec get_center() const { return center; }

    void set_radius( float new_radius ) { radius = new_radius; }
    float get_radius() const { return radius; }

private:
    Vec center;
    float radius;
}; 