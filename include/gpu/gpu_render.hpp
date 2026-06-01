#ifndef GPU_RENDER_HPP
#define GPU_RENDER_HPP

// Std includes
#include <vector>

// Lib includes
#include <glm/glm.hpp>

// Local includes
#include "util/types.h"
#include "core/scene.hpp"

namespace gpu {

struct float3 {
    float32 x, y, z;
    float3& operator=(const glm::vec3& v) {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }
};

inline glm::vec3 float3ToVec3(const float3 v) {
    return glm::vec3(v.x, v.y, v.z);
}

struct GPURenderParams {
    int32 screenW, screenH, samplesPerPixel, maxBounces;
    float3 camPos, pixelOrigin, pixelDeltaW, pixelDeltaH;
};

class GPURender {
public:
    GPURender() = default;
    GPURender(GPURenderParams params);

    std::vector<float3> renderScene(const core::Scene& scene);

private:
    GPURenderParams params;
};

} // namespace gpu

#endif // GPU_RENDER_HPP
