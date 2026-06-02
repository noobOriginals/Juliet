#include <lib/image.hpp>

// Lib includes
#include <stb/stb_image_write.h>

namespace lib {

Pixel makePixel(uint8 r, uint8 g, uint8 b) {
    Pixel p;
    p.r = r;
    p.g = g;
    p.b = b;
    return p;
}

Pixel makePixel(const glm::vec3& v) {
    Pixel p;
    p.r = v.x * 255;
    p.g = v.y * 255;
    p.b = v.z * 255;
    return p;
}

Image makeImage(int32 width, int32 height) {
    Image img;
    img.width = width;
    img.height = height;
    img.data = std::vector<Pixel>(width * height);
    return img;
}

void saveImage(const char* filepath, const Image& img) {
    stbi_flip_vertically_on_write(true);
    stbi_write_png(filepath, img.width, img.height, 3, img.data.data(), img.width * 3);
}

} // namespace lib