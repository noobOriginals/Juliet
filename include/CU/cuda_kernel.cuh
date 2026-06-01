#ifndef CU_CUDA_KERNEL_CUH
#define CU_CUDA_KERNEL_CUH

// Std includes
#include <cuda_runtime.h>

// Local includes
#include "cu/cuda_vec3.cuh"

#define OBJECT_DATA_SIZE 12
#define MATERIAL_DATA_SIZE 5

namespace cu {

struct Object {
    int type;
    float data[OBJECT_DATA_SIZE];
    int mIdx;
};

struct Material {
    int type;
    float data[MATERIAL_DATA_SIZE];
};

struct RenderParams {
    int32 screenW, screenH, samplesPerPixel, maxBounces;
    vec3 camPos, pixelOrigin, pixelDeltaW, pixelDeltaH;
};

struct SceneData {
    uint64 oCount, mCount;
};

__global__ void kernel(
    const RenderParams* p,
    const Object* objects,
    const Material* materials,
    const SceneData* sceneData,
    vec3* pixels
);

} // namespace cu

#endif // CU_CUDA_KERNEL_H
