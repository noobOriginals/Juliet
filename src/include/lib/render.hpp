#ifndef LIB_RENDERER_HPP
#define LIB_RENDERER_HPP

// Std includes
#include <string>

// Local includes
#include <lib/image.hpp>
#include <glm/glm.hpp>
#include <core/ray.hpp>
#include <core/scene.hpp>

namespace lib {

typedef glm::vec3 (*RenderRaytraceCallback)(const core::Ray& ray, int32 maxBounces);

struct RenderParameters {
    int32 screenWidth = 800;
    int32 screenHeight = 600;
    float32 vfov = 25.0f;
    float32 focalLength = 0.5f;

    glm::vec3 cameraPos;
    glm::vec3 cameraLookAt;
    glm::vec3 worldUp;

    int32 maxBounces = 10;
    int32 samplesPerPixel = 0;
    int32 threadTileSize = 100;
    bool enableSupersampling = false;
    bool enableGammaCorrection = false;
    bool enableMultiThreading = true;

    // Leave at 0 for default which is the number of CPU cores available on the computer
    int32 threadCount = 0;

    RenderRaytraceCallback raytraceCallback = nullptr;
};

class Render {
public:
    Render() = default;
    Render(RenderParameters params);

    void render(const core::Scene& scene) const;
    void save(std::string filepath) const;

private:
    struct Tile {
        int32 x0, y0, x1, y1;
    };

    Pixel renderPixel(int32 x, int32 y) const;
    void renderTile(Tile t) const;

    int32 screenW, screenH;
    glm::vec3 camPos, pixelDeltaW, pixelDeltaH, pixelOrigin;
    int32 maxBounces, samplesPerPixel, tileSize;
    bool supersampling, gammaCorrection, multiThreading;
    int32 threadCount;

    RenderRaytraceCallback raytraceCallback = nullptr;

    Image image;
};

} // namespace lib

#endif // LIB_RENDERER_HPP