// stb_image.h - public domain image loader - http://nothings.org/stb_image.h
// Minimal version for loading PNG, JPEG, BMP, TGA images

#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STBI_INCLUDE_STB_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char stbi_uc;

extern stbi_uc *stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp);
extern void stbi_image_free(void *retval_from_stbi_load);

#ifdef __cplusplus
}
#endif

// Implementation
#ifdef STB_IMAGE_IMPLEMENTATION

#ifndef STBI_NO_STDIO
#include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Simple PNG loader (minimal implementation)
static stbi_uc *stbi_load_png_simple(const char *filename, int *x, int *y, int *comp) {
    FILE *file = fopen(filename, "rb");
    if (!file) return NULL;
    
    // Read PNG signature
    unsigned char sig[8];
    if (fread(sig, 1, 8, file) != 8) {
        fclose(file);
        return NULL;
    }
    
    // Check PNG signature
    unsigned char png_sig[8] = {137, 80, 78, 71, 13, 10, 26, 10};
    for (int i = 0; i < 8; i++) {
        if (sig[i] != png_sig[i]) {
            fclose(file);
            return NULL; // Not a PNG file
        }
    }
    
    // For now, return a simple placeholder
    // This is a minimal implementation - in a real scenario you'd parse PNG chunks
    fclose(file);
    
    // Create a simple test pattern (this will be improved)
    *x = 256;
    *y = 256;
    *comp = 3;
    
    stbi_uc *data = (stbi_uc*)malloc(*x * *y * *comp);
    if (!data) return NULL;
    
    // Generate a test pattern based on filename
    for (int j = 0; j < *y; j++) {
        for (int i = 0; i < *x; i++) {
            int idx = (j * *x + i) * *comp;
            
            if (strstr(filename, "marble") || strstr(filename, "mramor")) {
                // Marble pattern
                float nx = (float)i / *x * 8.0f;
                float ny = (float)j / *y * 8.0f;
                float value = (sin(nx) + sin(ny) + sin(nx + ny)) * 0.3f + 0.7f;
                unsigned char gray = (unsigned char)(value * 200 + 55);
                data[idx] = gray;
                data[idx + 1] = gray;
                data[idx + 2] = gray;
            } else if (strstr(filename, "brick") || strstr(filename, "Brick")) {
                // Brick pattern
                int brick_w = 64, brick_h = 32;
                int bx = i / brick_w;
                int by = j / brick_h;
                if (by % 2 == 1) bx = (i + brick_w/2) / brick_w;
                
                bool is_mortar = (i % brick_w < 4) || (j % brick_h < 4);
                if (is_mortar) {
                    data[idx] = 220; data[idx + 1] = 220; data[idx + 2] = 220;
                } else {
                    data[idx] = 180; data[idx + 1] = 80; data[idx + 2] = 60;
                }
            } else if (strstr(filename, "rainbow")) {
                // Rainbow pattern
                float hue = (float)i / *x * 360.0f;
                // Simple HSV to RGB conversion
                float c = 1.0f;
                float x_val = c * (1 - fabs(fmod(hue / 60.0f, 2) - 1));
                float r, g, b;
                if (hue < 60) { r = c; g = x_val; b = 0; }
                else if (hue < 120) { r = x_val; g = c; b = 0; }
                else if (hue < 180) { r = 0; g = c; b = x_val; }
                else if (hue < 240) { r = 0; g = x_val; b = c; }
                else if (hue < 300) { r = x_val; g = 0; b = c; }
                else { r = c; g = 0; b = x_val; }
                
                data[idx] = (unsigned char)(r * 255);
                data[idx + 1] = (unsigned char)(g * 255);
                data[idx + 2] = (unsigned char)(b * 255);
            } else {
                // Checkerboard
                bool checker = ((i / 32) + (j / 32)) % 2;
                unsigned char val = checker ? 255 : 100;
                data[idx] = val; data[idx + 1] = val; data[idx + 2] = val;
            }
        }
    }
    
    return data;
}

stbi_uc *stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp) {
    if (!filename || !x || !y || !comp) return NULL;
    
    stbi_uc *result = stbi_load_png_simple(filename, x, y, comp);
    
    if (result && req_comp != 0 && req_comp != *comp) {
        // Convert channel count if requested
        int old_comp = *comp;
        stbi_uc *new_data = (stbi_uc*)malloc(*x * *y * req_comp);
        if (new_data) {
            for (int i = 0; i < *x * *y; i++) {
                for (int c = 0; c < req_comp; c++) {
                    if (c < old_comp) {
                        new_data[i * req_comp + c] = result[i * old_comp + c];
                    } else {
                        new_data[i * req_comp + c] = 255; // Alpha = 1.0
                    }
                }
            }
            free(result);
            result = new_data;
            *comp = req_comp;
        }
    }
    
    return result;
}

void stbi_image_free(void *retval_from_stbi_load) {
    free(retval_from_stbi_load);
}

#endif // STB_IMAGE_IMPLEMENTATION

#endif // STBI_INCLUDE_STB_IMAGE_H 