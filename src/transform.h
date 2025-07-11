#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "vec.h"
#include "mat4.h"
#include "ray.h"
#include "hit.h"

class Transform {
public:
    Transform() : m(Mat4::identity()), inv_m(Mat4::identity()) {}
    Transform(const Mat4& matrix) : m(matrix), inv_m(matrix.inverse()) {}

    Ray transform_ray(const Ray& ray) const {
        return Ray(transform_point(ray.origin), transform_direction(ray.direction));
    }

    Ray inverse_transform_ray(const Ray& ray) const {
        return Ray(inverse_transform_point(ray.origin), inverse_transform_direction(ray.direction));
    }

    Vec transform_point(const Vec& point) const {
        return m.transform_point(point);
    }

    Vec inverse_transform_point(const Vec& point) const {
        return inv_m.transform_point(point);
    }

    Vec transform_direction(const Vec& direction) const {
        return m.transform_direction(direction);
    }

    Vec inverse_transform_direction(const Vec& direction) const {
        return inv_m.transform_direction(direction);
    }

    Vec transform_normal(const Vec& normal) const {
        return inv_m.transpose().transform_direction(normal);
    }

    Vec inverse_transform_normal(const Vec& normal) const {
        return m.transpose().transform_direction(normal);
    }

    Hit transform_hit(const Hit& hit) const {
        Hit transformed;
        transformed.t = hit.t;
        transformed.point = transform_point(hit.point);
        transformed.normal = transform_normal(hit.normal);
        transformed.material = hit.material;
        transformed.u = hit.u;
        transformed.v = hit.v;
        return transformed;
    }

    Hit inverse_transform_hit(const Hit& hit) const {
        Hit transformed;
        transformed.t = hit.t;
        transformed.point = inverse_transform_point(hit.point);
        transformed.normal = inverse_transform_normal(hit.normal);
        transformed.material = hit.material;
        transformed.u = hit.u;
        transformed.v = hit.v;
        return transformed;
    }

    Vec get_scale() const {
        return m.get_scale();
    }

    static Transform translate(const Vec& v) {
        return Transform(Mat4::translate(v));
    }

    static Transform scale(const Vec& v) {
        return Transform(Mat4::scale(v));
    }

    static Transform rotateX(float theta) {
        return Transform(Mat4::rotateX(theta));
    }

    static Transform rotateY(float theta) {
        return Transform(Mat4::rotateY(theta));
    }

    static Transform rotateZ(float theta) {
        return Transform(Mat4::rotateZ(theta));
    }

    Transform operator*(const Transform& other) const {
        return Transform(m * other.m);
    }

private:
    Mat4 m;
    Mat4 inv_m;
};

#endif 