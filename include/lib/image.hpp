#ifndef LIB_IMAGE_HPP
#define LIB_IMAGE_HPP

// Std includes
#include <vector>

// Lib includes
#include <glm/glm.hpp>

// Local includes
#include "util/types.h"

namespace lib {

struct Pixel {
    uint8 r, g, b, a;
};

Pixel makePixel(uint8 r, uint8 g, uint8 b);
Pixel makePixel(uint8 r, uint8 g, uint8 b, uint8 a);
Pixel makePixel(const glm::vec3& v);
Pixel makePixel(const glm::vec4& v);

struct Image {
    int32 width, height;
    std::vector<Pixel> data;
};

Image makeImage(int32 width, int32 height);
void saveImage(const char* filepath, const Image& img);

}

#endif // LIB_IMAGE_HPP