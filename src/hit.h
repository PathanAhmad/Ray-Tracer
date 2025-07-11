#ifndef HIT_H
#define HIT_H

#include "vec.h"
#include "material.h"
#include <limits>

struct Hit {
    float t;
    Vec point;
    Vec normal;
    Material* material;
    float u, v;

    Hit() : t( std::numeric_limits<float>::max() ), normal(), point(), material( nullptr ), u( 0 ), v( 0 ) { }
};

#endif 