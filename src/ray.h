#ifndef RAY_H
#define RAY_H

#include "vec.h"

struct Ray {
    Vec origin;
    Vec direction;
    float min_t;
    float max_t;

    Ray ( ) : origin ( ), direction ( ), min_t(0.001f), max_t(1000.0f) { }
    Ray ( const Vec& origin, const Vec& direction ) : origin ( origin ), direction ( direction ), min_t(0.001f), max_t(1000.0f) { }
    Ray ( const Vec& origin, const Vec& direction, float min_t, float max_t ) : origin ( origin ), direction ( direction ), min_t(min_t), max_t(max_t) { }

    Vec point_at ( float t ) const {
        return origin + direction * t;
    }
};

#endif