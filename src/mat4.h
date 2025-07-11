#ifndef MAT4_H
#define MAT4_H

#include "vec.h"
#include <cmath>

class Mat4 {
public:
    Mat4() {
        for (int i = 0; i < 16; i++) {
            m[i] = 0.0f;
        }
    }

    float& operator()(int row, int col) {
        return m[row * 4 + col];
    }

    float operator()(int row, int col) const {
        return m[row * 4 + col];
    }

    static Mat4 identity() {
        Mat4 result;
        result(0, 0) = 1.0f;
        result(1, 1) = 1.0f;
        result(2, 2) = 1.0f;
        result(3, 3) = 1.0f;
        return result;
    }

    static Mat4 translate(const Vec& v) {
        Mat4 result = identity();
        result(0, 3) = v.x;
        result(1, 3) = v.y;
        result(2, 3) = v.z;
        return result;
    }

    static Mat4 scale(const Vec& v) {
        Mat4 result;
        result(0, 0) = v.x;
        result(1, 1) = v.y;
        result(2, 2) = v.z;
        result(3, 3) = 1.0f;
        return result;
    }

    static Mat4 rotateX(float theta) {
        float rad = theta * M_PI / 180.0f;
        float cos_theta = std::cos(rad);
        float sin_theta = std::sin(rad);
        Mat4 result = identity();
        result(1, 1) = cos_theta;
        result(1, 2) = -sin_theta;
        result(2, 1) = sin_theta;
        result(2, 2) = cos_theta;
        return result;
    }

    static Mat4 rotateY(float theta) {
        float rad = theta * M_PI / 180.0f;
        float cos_theta = std::cos(rad);
        float sin_theta = std::sin(rad);
        Mat4 result = identity();
        result(0, 0) = cos_theta;
        result(0, 2) = sin_theta;
        result(2, 0) = -sin_theta;
        result(2, 2) = cos_theta;
        return result;
    }

    static Mat4 rotateZ(float theta) {
        float rad = theta * M_PI / 180.0f;
        float cos_theta = std::cos(rad);
        float sin_theta = std::sin(rad);
        Mat4 result = identity();
        result(0, 0) = cos_theta;
        result(0, 1) = -sin_theta;
        result(1, 0) = sin_theta;
        result(1, 1) = cos_theta;
        return result;
    }

    Mat4 operator*(const Mat4& other) const {
        Mat4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                float sum = 0.0f;
                for (int k = 0; k < 4; k++) {
                    sum += (*this)(i, k) * other(k, j);
                }
                result(i, j) = sum;
            }
        }
        return result;
    }

    Vec transform_point(const Vec& point) const {
        float x = point.x * (*this)(0, 0) + point.y * (*this)(0, 1) + point.z * (*this)(0, 2) + (*this)(0, 3);
        float y = point.x * (*this)(1, 0) + point.y * (*this)(1, 1) + point.z * (*this)(1, 2) + (*this)(1, 3);
        float z = point.x * (*this)(2, 0) + point.y * (*this)(2, 1) + point.z * (*this)(2, 2) + (*this)(2, 3);
        float w = point.x * (*this)(3, 0) + point.y * (*this)(3, 1) + point.z * (*this)(3, 2) + (*this)(3, 3);
        return Vec(x / w, y / w, z / w);
    }

    Vec transform_direction(const Vec& direction) const {
        float x = direction.x * (*this)(0, 0) + direction.y * (*this)(0, 1) + direction.z * (*this)(0, 2);
        float y = direction.x * (*this)(1, 0) + direction.y * (*this)(1, 1) + direction.z * (*this)(1, 2);
        float z = direction.x * (*this)(2, 0) + direction.y * (*this)(2, 1) + direction.z * (*this)(2, 2);
        return Vec(x, y, z);
    }

    Mat4 transpose() const {
        Mat4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result(i, j) = (*this)(j, i);
            }
        }
        return result;
    }

    Mat4 inverse() const {
        Mat4 result;
        float det = determinant();
        if (std::abs(det) < 1e-6) {
            return identity();
        }
        float inv_det = 1.0f / det;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result(i, j) = cofactor(j, i) * inv_det;
            }
        }
        return result;
    }

    Vec get_scale() const {
        return Vec(
            std::sqrt((*this)(0, 0) * (*this)(0, 0) + (*this)(0, 1) * (*this)(0, 1) + (*this)(0, 2) * (*this)(0, 2)),
            std::sqrt((*this)(1, 0) * (*this)(1, 0) + (*this)(1, 1) * (*this)(1, 1) + (*this)(1, 2) * (*this)(1, 2)),
            std::sqrt((*this)(2, 0) * (*this)(2, 0) + (*this)(2, 1) * (*this)(2, 1) + (*this)(2, 2) * (*this)(2, 2))
        );
    }

private:
    float determinant() const {
        float det = 0.0f;
        for (int i = 0; i < 4; i++) {
            det += (*this)(0, i) * cofactor(0, i);
        }
        return det;
    }

    float cofactor(int row, int col) const {
        float minor[9];
        int index = 0;
        for (int i = 0; i < 4; i++) {
            if (i == row) continue;
            for (int j = 0; j < 4; j++) {
                if (j == col) continue;
                minor[index++] = (*this)(i, j);
            }
        }
        float det = minor[0] * (minor[4] * minor[8] - minor[5] * minor[7]) -
                   minor[1] * (minor[3] * minor[8] - minor[5] * minor[6]) +
                   minor[2] * (minor[3] * minor[7] - minor[4] * minor[6]);
        return ((row + col) % 2 == 0) ? det : -det;
    }

    float m[16];
};

#endif 