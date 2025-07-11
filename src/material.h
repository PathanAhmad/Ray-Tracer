#ifndef MATERIAL_H
#define MATERIAL_H

#include "vec.h"
#include <algorithm>
#include <string>
#include <cmath>
#include <vector>
#include <iostream>
#include <cstring>
#include "third_party/stb_image.h"

class Material {
public:
    Material() : 
        color(1, 1, 1),
        ka(0.1f),
        kd(0.7f),
        ks(0.2f),
        shininess(10.0f),
        reflection(0.0f),
        transmission(0.0f),
        ior(1.0f),
        is_textured(false),
        texture_data(nullptr),
        texture_width(0),
        texture_height(0),
        texture_channels(0) {}

    Material( const Vec& color, float ka, float kd, float ks, float shininess, float reflection, float transmission, float ior ) :
        color( color ),
        ka( ka ),
        kd( kd ),
        ks( ks ),
        shininess( shininess ),
        reflection( reflection ),
        transmission( transmission ),
        ior( ior ),
        is_textured(false),
        texture_data(nullptr),
        texture_width(0),
        texture_height(0),
        texture_channels(0) {}

    virtual ~Material() {
        if (texture_data) {
            delete[] texture_data;
        }
    }

    Vec color;
    float ka;  // Ambient coefficient
    float kd;  // Diffuse coefficient
    float ks;  // Specular coefficient
    float shininess;
    float reflection;
    float transmission;
    float ior;  // Index of refraction

    virtual Vec get_color ( float u, float v ) const {
        if ( is_textured && texture_data ) {
            // Here I wrap the coordinates for tiling
            u = u - floorf(u);
            v = v - floorf(v);
            
            // Here I convert to texture pixel locations
            int x = (int)(u * (texture_width - 1));
            int y = (int)(v * (texture_height - 1));
            
            // Get pixel from texture
            int index = (y * texture_width + x) * texture_channels;
            if (index >= 0 && index < texture_width * texture_height * texture_channels) {
                float r = texture_data[index] / 255.0f;
                float g = texture_data[index + 1] / 255.0f;
                float b = texture_data[index + 2] / 255.0f;
                
                // Here I convert the color values
                auto srgb_to_linear = [](float srgb) {
                    if (srgb <= 0.04045f) {
                        return srgb / 12.92f;
                    } else {
                        return std::pow((srgb + 0.055f) / 1.055f, 2.4f);
                    }
                };
                
                r = srgb_to_linear(r);
                g = srgb_to_linear(g);
                b = srgb_to_linear(b);
                
                // Here I return the processed colors
                return Vec(r, g, b);
            }
        }
        
        // Here I handle the fallback colors
        auto srgb_to_linear = [](float srgb) {
            if (srgb <= 0.04045f) {
                return srgb / 12.92f;
            } else {
                return std::pow((srgb + 0.055f) / 1.055f, 2.4f);
            }
        };
        
        float r = srgb_to_linear(color.x);
        float g = srgb_to_linear(color.y);
        float b = srgb_to_linear(color.z);
        
        return Vec(r, g, b);
    }

    bool load_texture(const std::string& filename) {
        std::cout << "Loading texture: " << filename << std::endl;
        
        // Clean up previous texture data
        if (texture_data) {
            delete[] texture_data;
            texture_data = nullptr;
        }
        
        // Try to load the actual image file
        std::string full_path = "scenes/" + filename;
        
        // Use STB to load the image
        int width, height, channels;
        unsigned char* loaded_data = stbi_load(full_path.c_str(), &width, &height, &channels, 3);
        
        if (loaded_data) {
            std::cout << "Successfully loaded texture: " << filename << " (" << width << "x" << height << ", " << channels << " channels)" << std::endl;
            
            texture_width = width;
            texture_height = height;
            texture_channels = 3; // Force RGB
            
            // Copy the loaded data
            int data_size = texture_width * texture_height * texture_channels;
            texture_data = new unsigned char[data_size];
            memcpy(texture_data, loaded_data, data_size);
            
            // Free the STB loaded data
            stbi_image_free(loaded_data);
            
            is_textured = true;
            return true;
        } else {
            std::cout << "Failed to load texture: " << filename << " - using default color" << std::endl;
            is_textured = false;
            return false;
        }
    }

private:
    // All procedural texture methods removed - we now use real image loading only

public:
    Vec shade ( const Vec& light_color, const Vec& normal, const Vec& light_dir, const Vec& view_dir ) const {
        Vec ambient = color * ka;
        Vec diffuse = color * kd * std::max ( 0.0f, Vec::dot( normal, light_dir ) );
        Vec R = normal * ( 2.0f * Vec::dot( normal, light_dir ) ) - light_dir;
        float rdotv = std::max ( 0.0f, Vec::dot( R, view_dir ) );
        Vec specular = color * ks * std::pow ( rdotv, shininess );
        return light_color * ( ambient + diffuse + specular );
    }

    Vec refract ( const Vec& I, const Vec& N, float ior ) const {
        float cosi = std::max ( -1.0f, std::min ( 1.0f, Vec::dot( I, N ) ) );
        float etai = 1, etat = ior;
        Vec n = N;
        if ( cosi < 0 ) {
            cosi = -cosi;
        } else {
            std::swap ( etai, etat );
            n = -N;
        }
        float eta = etai / etat;
        float k = 1 - eta * eta * ( 1 - cosi * cosi );
        if ( k < 0 ) {
            return Vec ( 0, 0, 0 );
        }
        return I * eta + n * ( eta * cosi - std::sqrt ( k ) );
    }

    Vec reflect ( const Vec& I, const Vec& N ) const {
        return I - N * ( 2.0f * Vec::dot( I, N ) );
    }

    float fresnel ( const Vec& I, const Vec& N ) const {
        float cosi = std::max ( -1.0f, std::min ( 1.0f, Vec::dot( I, N ) ) );
        float etai = 1, etat = ior;
        if ( cosi > 0 ) {
            std::swap ( etai, etat );
        }
        float sint = etai / etat * std::sqrt ( std::max ( 0.0f, 1 - cosi * cosi ) );
        if ( sint >= 1 ) {
            return 1;
        }
        float cost = std::sqrt ( std::max ( 0.0f, 1 - sint * sint ) );
        cosi = std::abs ( cosi );
        float Rs = ( ( etat * cosi ) - ( etai * cost ) ) / ( ( etat * cosi ) + ( etai * cost ) );
        float Rp = ( ( etai * cosi ) - ( etat * cost ) ) / ( ( etai * cosi ) + ( etat * cost ) );
        return ( Rs * Rs + Rp * Rp ) / 2;
    }

    bool is_tir ( const Vec& I, const Vec& N ) const {
        float cosi = std::max ( -1.0f, std::min ( 1.0f, Vec::dot( I, N ) ) );
        float etai = 1, etat = ior;
        if ( cosi > 0 ) {
            std::swap ( etai, etat );
        }
        float sint = etai / etat * std::sqrt ( std::max ( 0.0f, 1 - cosi * cosi ) );
        return sint >= 1;
    }

    Vec get_diffuse ( ) const {
        return color;
    }

    Vec get_reflection ( ) const {
        return Vec::reflect ( Vec ( 0, 0, 0 ), Vec ( 0, 0, 0 ) );
    }

    float get_shininess ( ) const {
        return shininess;
    }

    bool is_textured;
    std::string texture_name;

protected:
    unsigned char* texture_data;
    int texture_width;
    int texture_height;
    int texture_channels;
};

struct TexturedMaterial : public Material {
    TexturedMaterial ( const std::string& texture_file,
                      float ka = 0.1f,
                      float kd = 0.7f,
                      float ks = 0.2f,
                      float exp = 10.0f,
                      float r = 0.0f,
                      float t = 0.0f,
                      float ior = 1.0f )
        : Material ( Vec ( 1, 1, 1 ), ka, kd, ks, exp, r, t, ior ),
          texture_file ( texture_file ) { 
        is_textured = true;
        texture_name = texture_file;
        load_texture(texture_file);
    }

    std::string texture_file;
};

#endif 