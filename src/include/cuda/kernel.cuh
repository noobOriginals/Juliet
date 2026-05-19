#ifndef CUDA_KERNEL_CUH
#define CUDA_KERNEL_CUH

// Std includes
#include <cuda_runtime.h>
#include <curand_kernel.h>

// Local includes
#include <util/types.h>
#include <cuda/vec3.cuh>
#include <cuda/scene.cuh>

namespace cuda {

__global__ void renderScene(
    curandStatePhilox4_32_10* randStates,
    uint64 numStates,
    vec3* pixels,
    int32 screenW,
    int32 screenH,
    vec3 camPos,
    vec3 pixelOrigin,
    vec3 pixelDeltaW,
    vec3 pixelDeltaH,
    int32 maxBounces,
    int32 samplesPerPixel,
    Scene scene
);

} // namespace cuda

#endif // CUDA_KERNEL_CUH
