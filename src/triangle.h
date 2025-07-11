#pragma once
#include "vec.h"
#include "ray.h"
#include "material.h"
#include "transform.h"
#include "object.h"
#include "hit.h"
#include <cmath>
#include <iostream>

class Triangle : public Object {
public:
    Triangle( const Vec& a, const Vec& b, const Vec& c, Material* mat ) : v0(a), v1(b), v2(c) {
        normal = Vec::cross( v1 - v0, v2 - v0 ).normalize();
        material = mat;
    }

    void set_normals( const Vec& n0, const Vec& n1, const Vec& n2 ) {
        normals[0] = n0;
        normals[1] = n1;
        normals[2] = n2;
        has_normals = true;
    }

    void set_tex_coords( const Vec& t0, const Vec& t1, const Vec& t2 ) {
        texcoords[0] = t0;
        texcoords[1] = t1;
        texcoords[2] = t2;
        has_texcoords = true;
    }

    bool intersect( const Ray& ray, Hit& hit ) const override {
        Vec edge1 = v1 - v0;
        Vec edge2 = v2 - v0;
        Vec h = Vec::cross( ray.direction, edge2 );
        float a = Vec::dot( edge1, h );

        // Use consistent epsilon with rest of codebase (0.001f instead of 0.00001)
        if ( a > -0.001f && a < 0.001f ) {
            return false;
        }

        float f = 1.0 / a;
        Vec s = ray.origin - v0;
        float u = f * Vec::dot( s, h );

        if ( u < 0.0 || u > 1.0 ) {
            return false;
        }

        Vec q = Vec::cross( s, edge1 );
        float v = f * Vec::dot( ray.direction, q );

        if ( v < 0.0 || u + v > 1.0 ) {
            return false;
        }

        float t = f * Vec::dot( edge2, q );

        // Use consistent epsilon with rest of codebase (0.001f instead of 0.00001)
        if ( t > 0.001f ) {
            hit.t = t;
            hit.point = ray.origin + ray.direction * t;
            
            Vec n = Vec::cross( edge1, edge2 ).normalize();
            
            if ( Vec::dot( n, ray.direction ) > 0 ) {
                n = n * -1.0;
            }
            
            hit.normal = n;
            hit.material = material;
            
            // Here I set the surface coordinates
            if ( has_texcoords ) {
                float w = 1.0f - u - v;
                Vec tex_interpolated = w * texcoords[0] + u * texcoords[1] + v * texcoords[2];
                hit.u = tex_interpolated.x;
                hit.v = tex_interpolated.y;
            } else {
                hit.u = u;
                hit.v = v;
            }
            
            return true;
        }

        return false;
    }

    Vec get_normal( const Vec& point ) const override {
        if ( has_normals ) {
            // Calculate barycentric coordinates
            Vec e1 = v1 - v0;
            Vec e2 = v2 - v0;
            Vec p = point - v0;
            float d00 = Vec::dot( e1, e1 );
            float d01 = Vec::dot( e1, e2 );
            float d11 = Vec::dot( e2, e2 );
            float d20 = Vec::dot( p, e1 );
            float d21 = Vec::dot( p, e2 );
            float denom = d00 * d11 - d01 * d01;
            float v = ( d11 * d20 - d01 * d21 ) / denom;
            float w = ( d00 * d21 - d01 * d20 ) / denom;
            float u = 1.0f - v - w;

            // Interpolate normal
            return ( u * normals[0] + v * normals[1] + w * normals[2] ).normalize();
        }
        return normal;
    }

    bool contains_point( const Vec& point ) const {
        Vec e1 = v1 - v0;
        Vec e2 = v2 - v0;
        Vec p = point - v0;
        float d00 = Vec::dot( e1, e1 );
        float d01 = Vec::dot( e1, e2 );
        float d11 = Vec::dot( e2, e2 );
        float d20 = Vec::dot( p, e1 );
        float d21 = Vec::dot( p, e2 );
        float denom = d00 * d11 - d01 * d01;
        float v = ( d11 * d20 - d01 * d21 ) / denom;
        float w = ( d00 * d21 - d01 * d20 ) / denom;
        float u = 1.0f - v - w;

        return u >= 0.0f && v >= 0.0f && w >= 0.0f;
    }

    Vec2 get_tex_coords( const Vec& point ) const {
        Vec edge1 = v1 - v0;
        Vec edge2 = v2 - v0;
        Vec p = point - v0;
        float d00 = edge1.dot( edge1 );
        float d01 = edge1.dot( edge2 );
        float d11 = edge2.dot( edge2 );
        float d20 = p.dot( edge1 );
        float d21 = p.dot( edge2 );
        float denom = d00 * d11 - d01 * d01;
        float v = ( d11 * d20 - d01 * d21 ) / denom;
        float w = ( d00 * d21 - d01 * d20 ) / denom;
        float u = 1.0f - v - w;
        Vec interpolated = u * texcoords[0] + v * texcoords[1] + w * texcoords[2];
        return Vec2( interpolated.x, interpolated.y );
    }

    void set_vertices( const Vec& new_v0, const Vec& new_v1, const Vec& new_v2 ) {
        v0 = new_v0;
        v1 = new_v1;
        v2 = new_v2;
        normal = Vec::cross( v1 - v0, v2 - v0 ).normalize();
    }

    void get_vertices_and_normal( Vec& out_v0, Vec& out_v1, Vec& out_v2, Vec& out_normal ) const {
        out_v0 = v0;
        out_v1 = v1;
        out_v2 = v2;
        out_normal = normal;
    }

private:
    Vec v0, v1, v2;
    Vec normal;
    Vec normals[3];
    Vec texcoords[3];
    bool has_normals = false;
    bool has_texcoords = false;
}; 