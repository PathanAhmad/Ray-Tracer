#ifndef OBJECT_H
#define OBJECT_H

#include "ray.h"
#include "material.h"
#include "transform.h"
#include "hit.h"

class Object {
public:
    Object() : material(nullptr) {}
    Object( Material* mat ) : material( mat ) {}
    virtual ~Object() {}

    virtual bool intersect( const Ray& ray, Hit& hit ) const = 0;
    virtual Vec get_normal( const Vec& point ) const = 0;

    Material* material;
    Transform transform;
};

#endif 