// Std includes
#include <iostream>

// Lib includes
#include <glm/glm.hpp>
#include <stb_image/stb_image_write.h>

// Local includes
#include <util/util.hpp>
#include <core/object.hpp>

using namespace glm;
using namespace util;

#define IMAGE_WIDTH 800
#define IMAGE_HEIGHT 600
#define VFOV 25.0f

int main() {
    vec3 worldUp(0, 1, 0);

    vec3 pos(0.0f, 1.5f, 4.0f);
    vec3 lookat(0.0f, 0.5f, 0.0f);

    vec3 dir = normalize(lookat - pos);
    vec3 right = normalize(cross(dir, worldUp));
    vec3 up = normalize(cross(right, dir));

    float32 focal = 0.5f;
    float32 height = std::tan(degToRad(VFOV * 0.5f)) * focal;
    float32 width = height * IMAGE_WIDTH / IMAGE_HEIGHT;

    vec3 pixelDeltaX = right * (2.0f * width / IMAGE_WIDTH);
    vec3 pixelDeltaY = up * (2.0f * height / IMAGE_HEIGHT);
    vec3 pixelOrigin = pos + dir * focal - right * width - up * height + pixelDeltaX * 0.5f + pixelDeltaY * 0.5f;

    uint8 pixels[IMAGE_WIDTH * IMAGE_HEIGHT * 3];

    core::Object sphere = core::makeSphere(vec3(0.0f, 0.5f, 0.0f), 0.5f);

    for (int32 y = 0; y < IMAGE_HEIGHT; y++) {
        for (int32 x = 0; x < IMAGE_WIDTH; x++) {
            core::Ray ray;
            core::HitRecord hit;
            ray.org = pos;
            ray.dir = glm::normalize(pixelOrigin + pixelDeltaX * (float32)x + pixelDeltaY * (float32)y - pos);
            if (core::hitSphere(ray, hit, 1e-4f, std::numeric_limits<float32>::max(), sphere.data)) {
                pixels[(y * IMAGE_WIDTH + x) * 3] = 255;
                pixels[(y * IMAGE_WIDTH + x) * 3 + 1] = 255;
                pixels[(y * IMAGE_WIDTH + x) * 3 + 2] = 255;
            } else {
                pixels[(y * IMAGE_WIDTH + x) * 3] = 0;
                pixels[(y * IMAGE_WIDTH + x) * 3 + 1] = 0;
                pixels[(y * IMAGE_WIDTH + x) * 3 + 2] = 0;
            }
        }
    }

    stbi_flip_vertically_on_write(true);
    stbi_write_png("renders/render.png", IMAGE_WIDTH, IMAGE_HEIGHT, 3, pixels, IMAGE_WIDTH * 3);
    return 0;
}
