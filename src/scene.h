#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "object.h"
#include "light.h"
#include "material.h"
#include "third_party/stb_image_write.h"
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>

class SceneParser;

class Scene {
public:
    Scene() : max_bounces(5) {}

    ~Scene() {
        for ( Object* obj : objects ) {
            delete obj;
        }
        for ( Light* light : lights ) {
            delete light;
        }
        for ( Material* mat : materials ) {
            delete mat;
        }
    }

    bool load( const std::string& filename );

    void render( const std::string& output_filename ) {
        output_file = output_filename;
        std::vector<Vec> pixels( camera.width * camera.height );
        const int samples_per_pixel = 4;

        std::cout << "Rendering " << camera.width << "x" << camera.height << " image..." << std::endl;

        for ( int y = 0; y < camera.height; y++ ) {
            // Progress output every 10% of rows
            if ( y % (camera.height / 10) == 0 ) {
                int progress = (y * 100) / camera.height;
                std::cout << "Progress: " << progress << "% (row " << y << "/" << camera.height << ")" << std::endl;
            }
            
            for ( int x = 0; x < camera.width; x++ ) {
                Vec pixel_color( 0, 0, 0 );
                
                for ( int sy = 0; sy < 2; sy++ ) {
                    for ( int sx = 0; sx < 2; sx++ ) {
                        float offset_x = ( sx + 0.5f ) / 2.0f;
                        float offset_y = ( sy + 0.5f ) / 2.0f;
                        
                        Ray ray = camera.get_ray( x + offset_x, y + offset_y );
                        ray.min_t = 0.001f;  // Avoid self-intersection
                        ray.max_t = 1000.0f; // Reasonable scene bounds
                        Vec sample_color = trace_ray( ray, 0 );
                        pixel_color = pixel_color + sample_color;
                    }
                }
                
                // Here I handle the coordinate system
                int flipped_y = camera.height - 1 - y;
                pixels[flipped_y * camera.width + x] = pixel_color * ( 1.0f / samples_per_pixel );
            }
        }

        std::cout << "Rendering complete. Saving image..." << std::endl;
        save_image( pixels );
        std::cout << "Image saved to: " << output_filename << std::endl;
    }

    void save_image( const std::vector<Vec>& pixels ) {
        std::vector<unsigned char> data( pixels.size() * 3 );
        for ( size_t i = 0; i < pixels.size(); i++ ) {
            // Clamp values
            float r = std::max( 0.0f, std::min( 1.0f, pixels[i].x ) );
            float g = std::max( 0.0f, std::min( 1.0f, pixels[i].y ) );
            float b = std::max( 0.0f, std::min( 1.0f, pixels[i].z ) );
            
            // Here I prepare colors for display
            r = std::pow( r, 1.0f / 2.2f );
            g = std::pow( g, 1.0f / 2.2f );
            b = std::pow( b, 1.0f / 2.2f );
            
            data[i * 3] = (unsigned char)( r * 255 );
            data[i * 3 + 1] = (unsigned char)( g * 255 );
            data[i * 3 + 2] = (unsigned char)( b * 255 );
        }
        
        // Create output directory if it doesn't exist
        std::filesystem::path file_path( output_file );
        std::filesystem::create_directories( file_path.parent_path() );
        
        stbi_write_png( output_file.c_str(), camera.width, camera.height, 3, data.data(), camera.width * 3 );
    }

    std::string output_file;
    Vec background_color;
    Camera camera;
    std::vector<Object*> objects;
    std::vector<Light*> lights;
    std::vector<Material*> materials;
    Vec ambientLight;
    int max_bounces;

private:
    Vec trace_ray( const Ray& ray, int depth = 0 ) {
        if ( depth > max_bounces ) {
            return background_color;
        }

        Hit hit;
        if ( !intersect( ray, hit ) ) {
            return background_color;
        }

        Vec color = Vec( 0, 0, 0 );
        Vec original_normal = hit.normal.normalize();  // Keep the original normal for refraction
        Vec normal = original_normal;
        Vec point = hit.point;
        Vec view_dir = -ray.direction.normalize();

        // Here I fix the normal direction for lighting calculations
        if ( Vec::dot( normal, view_dir ) < 0 ) {
            normal = -normal;
        }

        // Calculate local lighting (ambient + direct lighting)
        Vec local_color = Vec( 0, 0, 0 );
        
        // Start with ambient light
        for ( Light* light : lights ) {
            if ( dynamic_cast<AmbientLight*>( light ) ) {
                Vec light_intensity = light->get_intensity( point );
                // Here I get the surface color
                if ( hit.material ) {
                    Vec surface_color = hit.material->get_color(hit.u, hit.v);
                    Vec ambient_contribution = surface_color * hit.material->ka * light_intensity;
                    local_color = local_color + ambient_contribution;
                }
            }
        }

        // Calculate lighting for non-ambient lights
        for ( Light* light : lights ) {
            if ( dynamic_cast<AmbientLight*>( light ) ) {
                continue;
            }
            
            Vec light_dir = light->get_direction( point );
            float light_dist = light->get_distance( point );
            Vec light_intensity = light->get_intensity( point );

            // Here I check for shadows
            Ray shadow_ray( point + normal * 0.001f, light_dir );
            shadow_ray.min_t = 0.001f;
            // For parallel lights, use very large distance; for point lights, use actual distance
            shadow_ray.max_t = std::isinf(light_dist) ? 1000.0f : light_dist - 0.001f;
            Hit shadow_hit;
            shadow_hit.t = INFINITY;
            
            bool in_shadow = false;
            if ( intersect( shadow_ray, shadow_hit ) ) {
                in_shadow = true; // Any intersection within ray bounds means shadow
            }
            
            if ( !in_shadow && hit.material ) {
                // Get surface color from texture
                Vec surface_color = hit.material->get_color(hit.u, hit.v);
                
                // Diffuse lighting
                float diffuse_factor = std::max( 0.0f, Vec::dot( normal, light_dir ) );
                Vec diffuse_color = surface_color * hit.material->kd * diffuse_factor * light_intensity;
                local_color = local_color + diffuse_color;

                // Here I add the shiny highlights
                Vec half_vector = ( light_dir + view_dir ).normalize();
                float specular_factor = std::pow( std::max( 0.0f, Vec::dot( normal, half_vector ) ), hit.material->shininess );
                Vec specular_color = light_intensity * hit.material->ks * specular_factor;
                local_color = local_color + specular_color;
            }
        }

        // Calculate total surface contribution factor
        float surface_factor = 1.0f;
        if ( hit.material ) {
            surface_factor = 1.0f - hit.material->transmission - hit.material->reflection;
            surface_factor = std::max( 0.0f, surface_factor );
        }
        
        // Start with local lighting (reduced by transparency and reflection)
        color = local_color * surface_factor;

        // Reflection
        if ( hit.material && hit.material->reflection > 0 ) {
            Vec reflect_dir = Vec::reflect( ray.direction, original_normal );
            Ray reflect_ray( point + original_normal * 0.001f, reflect_dir );
            reflect_ray.min_t = 0.001f;
            reflect_ray.max_t = 1000.0f;
            Vec reflected_color = trace_ray( reflect_ray, depth + 1 );
            color = color + reflected_color * hit.material->reflection;
        }

        // Transmission
        if ( hit.material && hit.material->transmission > 0 ) {
            // Here I get the original normal for refraction (not the flipped one)
            Vec refraction_normal = original_normal;
            
            // Here I calculate the refracted ray
            Vec refract_dir = hit.material->refract( ray.direction, refraction_normal, hit.material->ior );
            
            if ( refract_dir != Vec( 0, 0, 0 ) ) {
                // Here I determine the ray offset direction
                bool entering = Vec::dot( ray.direction, refraction_normal ) < 0;
                Vec offset_normal = entering ? -refraction_normal : refraction_normal;
                
                Ray refract_ray( point + offset_normal * 0.001f, refract_dir );
                refract_ray.min_t = 0.001f;
                refract_ray.max_t = 1000.0f;
                Vec refracted_color = trace_ray( refract_ray, depth + 1 );
                color = color + refracted_color * hit.material->transmission;
            } else {
                // Total internal reflection - use reflection instead of transmission
                Vec reflect_dir = Vec::reflect( ray.direction, refraction_normal );
                Ray reflect_ray( point + refraction_normal * 0.001f, reflect_dir );
                reflect_ray.min_t = 0.001f;
                reflect_ray.max_t = 1000.0f;
                Vec reflected_color = trace_ray( reflect_ray, depth + 1 );
                color = color + reflected_color * hit.material->transmission;
            }
        }

        return color;
    }

    bool intersect( const Ray& ray, Hit& hit ) {
        bool found = false;
        float closest_t = INFINITY;

        for ( Object* obj : objects ) {
            Hit temp_hit;
            temp_hit.t = INFINITY;
            
            if ( obj->intersect( ray, temp_hit ) ) {
                if ( temp_hit.t < closest_t && temp_hit.t >= ray.min_t && temp_hit.t <= ray.max_t ) {
                    closest_t = temp_hit.t;
                    hit = temp_hit;
                    found = true;
                }
            }
        }

        return found;
    }
};

#endif