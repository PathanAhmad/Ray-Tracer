#ifndef VEC_H
#define VEC_H

#include <cmath>

struct Vec3 {
    float x, y, z;

    Vec3 ( ) : x ( 0 ), y ( 0 ), z ( 0 ) { }
    Vec3 ( float x, float y, float z ) : x ( x ), y ( y ), z ( z ) { }
    Vec3 ( const Vec3& v ) : x ( v.x ), y ( v.y ), z ( v.z ) { }

    Vec3& operator= ( const Vec3& v ) {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    Vec3 operator+ ( const Vec3& v ) const {
        return Vec3 ( x + v.x, y + v.y, z + v.z );
    }

    Vec3 operator- ( const Vec3& v ) const {
        return Vec3 ( x - v.x, y - v.y, z - v.z );
    }

    Vec3 operator* ( float s ) const {
        return Vec3 ( x * s, y * s, z * s );
    }

    Vec3 operator/ ( float s ) const {
        return Vec3 ( x / s, y / s, z / s );
    }

    Vec3 operator- ( ) const {
        return Vec3 ( -x, -y, -z );
    }

    float dot ( const Vec3& v ) const {
        return x * v.x + y * v.y + z * v.z;
    }

    Vec3 cross ( const Vec3& v ) const {
        return Vec3 ( y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x );
    }

    float length ( ) const {
        return std::sqrt ( x * x + y * y + z * z );
    }

    Vec3 normalize ( ) const {
        float len = length ( );
        if ( len == 0 ) return Vec3 ( );
        return *this / len;
    }

    Vec3 operator* ( const Vec3& v ) const {
        return Vec3 ( x * v.x, y * v.y, z * v.z );
    }
};

inline Vec3 operator* ( float s, const Vec3& v ) {
    return v * s;
}

inline Vec3 operator/ ( float s, const Vec3& v ) {
    return Vec3 ( s / v.x, s / v.y, s / v.z );
}

struct Vec {
    float x, y, z;

    Vec ( ) : x ( 0 ), y ( 0 ), z ( 0 ) { }
    Vec ( float x, float y, float z ) : x ( x ), y ( y ), z ( z ) { }
    Vec ( const Vec& v ) : x ( v.x ), y ( v.y ), z ( v.z ) { }
    Vec ( const Vec3& v ) : x ( v.x ), y ( v.y ), z ( v.z ) { }

    Vec& operator= ( const Vec& v ) {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    Vec& operator= ( const Vec3& v ) {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    Vec operator+ ( const Vec& v ) const {
        return Vec ( x + v.x, y + v.y, z + v.z );
    }

    Vec operator- ( const Vec& v ) const {
        return Vec ( x - v.x, y - v.y, z - v.z );
    }

    Vec operator* ( float s ) const {
        return Vec ( x * s, y * s, z * s );
    }

    Vec operator/ ( float s ) const {
        return Vec ( x / s, y / s, z / s );
    }

    Vec operator- ( ) const {
        return Vec ( -x, -y, -z );
    }

    float dot ( const Vec& v ) const {
        return x * v.x + y * v.y + z * v.z;
    }

    Vec cross ( const Vec& v ) const {
        return Vec ( y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x );
    }

    float length ( ) const {
        return std::sqrt ( x * x + y * y + z * z );
    }

    Vec normalize ( ) const {
        float len = length ( );
        if ( len == 0 ) return Vec ( );
        return *this / len;
    }

    operator Vec3 ( ) const {
        return Vec3 ( x, y, z );
    }

    bool operator==(const Vec& v) const { return x == v.x && y == v.y && z == v.z; }
    bool operator!=(const Vec& v) const { return !(*this == v); }
    Vec operator*(const Vec& v) const { return Vec(x * v.x, y * v.y, z * v.z); }

    Vec& operator+=(const Vec& v) { x = x + v.x; y = y + v.y; z = z + v.z; return *this; }
    Vec& operator-=(const Vec& v) { x = x - v.x; y = y - v.y; z = z - v.z; return *this; }
    Vec& operator*=(float s) { x = x * s; y = y * s; z = z * s; return *this; }
    Vec& operator/=(float s) { x = x / s; y = y / s; z = z / s; return *this; }

    float length_squared() const { return x * x + y * y + z * z; }
    static float dot(const Vec& a, const Vec& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
    static Vec cross(const Vec& a, const Vec& b) {
        return Vec(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }
    static Vec reflect(const Vec& I, const Vec& N) {
        return I - N * (2.0f * dot(I, N));
    }
    static Vec refract(const Vec& I, const Vec& N, float eta) {
        float d = dot(I, N);
        float k = 1.0f - eta * eta * (1.0f - d * d);
        if (k < 0.0f) return Vec();
        return I * eta - N * (eta * d + std::sqrt(k));
    }
};

inline Vec operator* ( float s, const Vec& v ) {
    return v * s;
}

inline Vec operator/ ( float s, const Vec& v ) {
    return Vec ( s / v.x, s / v.y, s / v.z );
}

struct Vec2 {
    float x, y;

    Vec2 ( ) : x ( 0 ), y ( 0 ) { }
    Vec2 ( float x, float y ) : x ( x ), y ( y ) { }
    Vec2 ( const Vec2& v ) : x ( v.x ), y ( v.y ) { }

    Vec2& operator= ( const Vec2& v ) {
        x = v.x;
        y = v.y;
        return *this;
    }

    Vec2 operator+ ( const Vec2& v ) const {
        return Vec2 ( x + v.x, y + v.y );
    }

    Vec2 operator- ( const Vec2& v ) const {
        return Vec2 ( x - v.x, y - v.y );
    }

    Vec2 operator* ( float s ) const {
        return Vec2 ( x * s, y * s );
    }

    Vec2 operator/ ( float s ) const {
        return Vec2 ( x / s, y / s );
    }

    Vec2 operator- ( ) const {
        return Vec2 ( -x, -y );
    }

    float dot ( const Vec2& v ) const {
        return x * v.x + y * v.y;
    }

    float length ( ) const {
        return std::sqrt ( x * x + y * y );
    }

    Vec2 normalize ( ) const {
        float len = length ( );
        if ( len > 0 ) {
            return *this / len;
        }
        return *this;
    }
};

inline Vec2 operator* ( float s, const Vec2& v ) {
    return v * s;
}

#endif
