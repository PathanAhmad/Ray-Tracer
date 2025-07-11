#ifndef IMAGE_H
#define IMAGE_H

#include <vector>
#include <string>
#include <cstdio>
#include <algorithm>
#include "vec.h"

class Image {
public:
    int width, height;
    std::vector<unsigned char> data;

    Image(int w, int h) : width(w), height(h), data(w * h * 3, 0) { }

    void setPixel(int x, int y, const Vec& color) {
        int idx = (y * width + x) * 3;
        data[idx] = (unsigned char)(std::min(1.0f, std::max(0.0f, color.x)) * 255.0f);
        data[idx + 1] = (unsigned char)(std::min(1.0f, std::max(0.0f, color.y)) * 255.0f);
        data[idx + 2] = (unsigned char)(std::min(1.0f, std::max(0.0f, color.z)) * 255.0f);
    }

    void savePNG(const std::string& filename) const {
        FILE* f = fopen(filename.c_str(), "wb");
        if (!f) return;
        fprintf(f, "P6\n%d %d\n255\n", width, height);
        fwrite(data.data(), 1, width * height * 3, f);
        fclose(f);
    }
};

#endif 