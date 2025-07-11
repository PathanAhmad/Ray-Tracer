#ifndef CAMERA_H
#define CAMERA_H

#include "ray.h"
#include "vec.h"
#include <cmath>
#include <cstdio>

#define M_PI 3.14159265358979323846

class Camera {
public:
    Camera() : width(800), height(600), fov(45.0f) {}

    Ray get_ray( float x, float y ) const {
        // Standard raytracing camera model
        // 1. Convert to normalized device coordinates [0,1]
        float u = (x + 0.5f) / width;
        float v = (y + 0.5f) / height;
        
        // 2. Convert to screen coordinates [-1,1]
        float screen_x = 2.0f * u - 1.0f;
        float screen_y = 2.0f * v - 1.0f;
        
        // 3. Apply FOV and aspect ratio with more aggressive scaling
        float aspect = (float)width / height;
        float fov_radians = fov * M_PI / 180.0f;
        // Try 2.5x scale for perfect zoom level
        float scale = 2.5f * std::tan(fov_radians / 2.0f);
        
        // Apply scaling and aspect ratio
        screen_x *= scale * aspect;
        screen_y *= scale;
        
        // 4. Ray direction in camera space (pointing towards -z)
        Vec camera_ray_dir = Vec(screen_x, screen_y, -1.0f).normalize();
        
        // 5. Transform to world space using camera orientation
        Vec camera_forward = (look_at - position).normalize();
        Vec camera_right = Vec::cross(camera_forward, up).normalize();
        Vec camera_up = Vec::cross(camera_right, camera_forward).normalize();
        
        // Transform ray direction to world space
        Vec world_ray_dir = camera_ray_dir.x * camera_right + 
                           camera_ray_dir.y * camera_up + 
                           (-camera_ray_dir.z) * camera_forward;
        world_ray_dir = world_ray_dir.normalize();

        return Ray( position, world_ray_dir );
    }

    Vec position;
    Vec look_at;
    Vec up;
    float fov;
    int width;
    int height;

private:
    float aspect_ratio() const {
        return ( float )width / height;
    }
};

#endif 