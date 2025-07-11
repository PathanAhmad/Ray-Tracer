#ifndef LIGHT_H
#define LIGHT_H

#include "vec.h"
#include <cmath>

#define M_PI 3.14159265358979323846

class Light {
public:
    Light( const Vec& c ) : color(c) {}
    virtual ~Light() {}

    virtual Vec get_direction( const Vec& point ) const = 0;
    virtual float get_distance( const Vec& point ) const = 0;
    virtual Vec get_intensity( const Vec& point ) const = 0;

protected:
    Vec color;
};

class AmbientLight : public Light {
public:
    AmbientLight( const Vec& c ) : Light(c) {}

    Vec get_direction( const Vec& point ) const override {
        return Vec( 0, 0, 0 );
    }

    float get_distance( const Vec& point ) const override {
        return 0.0f;
    }

    Vec get_intensity( const Vec& point ) const override {
        return color;
    }
};

class PointLight : public Light {
public:
    PointLight( const Vec& p, const Vec& c ) : Light(c), position(p) {}

    Vec get_direction( const Vec& point ) const override {
        return (position - point).normalize();
    }

    float get_distance( const Vec& point ) const override {
        return ( position - point ).length();
    }

    Vec get_intensity( const Vec& point ) const override {
        float dist = get_distance( point );
        // Here I calculate the light falloff
        return color / ( 1.0f + 0.09f * dist + 0.032f * dist * dist );
    }

private:
    Vec position;
};

class ParallelLight : public Light {
public:
    ParallelLight( const Vec& d, const Vec& c ) : Light(c), direction((-d).normalize()) {}

    Vec get_direction( const Vec& point ) const override {
        return direction;
    }

    float get_distance( const Vec& point ) const override {
        return INFINITY;
    }

    Vec get_intensity( const Vec& point ) const override {
        return color;
    }

private:
    Vec direction;
};

class SpotLight : public Light {
public:
    SpotLight( const Vec& p, const Vec& d, const Vec& c, float a1, float a2 )
        : Light(c), position(p), direction(d.normalize()), angle1(a1), angle2(a2) {}

    Vec get_direction( const Vec& point ) const override {
        return (position - point).normalize();
    }

    float get_distance( const Vec& point ) const override {
        return ( position - point ).length();
    }

    Vec get_intensity( const Vec& point ) const override {
        Vec dir = get_direction( point );
        float cos_angle = Vec::dot( -dir, direction );
        float angle = std::acos( cos_angle ) * 180.0f / M_PI;
        
        float dist = get_distance( point );
        // Here I calculate the distance attenuation
        float attenuation = 1.0f + 0.09f * dist + 0.032f * dist * dist;

        if ( angle <= angle1 ) {
            return color / attenuation;
        } else if ( angle <= angle2 ) {
            float t = ( angle - angle1 ) / ( angle2 - angle1 );
            return color * ( 1.0f - t ) / attenuation;
        }
        return Vec( 0, 0, 0 );
    }

private:
    Vec position;
    Vec direction;
    float angle1;
    float angle2;
};

#endif 