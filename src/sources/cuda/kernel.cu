#include <cuda/kernel.cuh>

// Local includes
#include <cuda/random.cuh>
#include <cuda/optics.cuh>
#include <cuda/ray.cuh>
#include <cuda/hitrecord.cuh>
#include <cuda/material.cuh>

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
) {
    int32 x = threadIdx.x + blockIdx.x * blockDim.x;
    int32 y = threadIdx.y + blockIdx.y * blockDim.y;
    int32 idx = y * screenW + x;
    if (idx > screenW * screenH || idx > numStates) {
        return;
    }
    curandStatePhilox4_32_10 localRandState = randStates[idx];

    Ray ray;
    HitRecord hit;
    ray.org = camPos;
    vec3 pixel = pixelOrigin + pixelDeltaW * x + pixelDeltaH * y;
    vec3 totalColor(0.0f);
    for (int32 i = 0; i < samplesPerPixel; i++) {
        vec3 jitter = pixelDeltaW * (nextGaussian(&localRandState) - 0.5f) + pixelDeltaH * (nextGaussian(&localRandState) - 0.5f);
        ray.dir = normalize(pixel + jitter - camPos);
        Ray r = ray;
        int32 matIdx;
        vec3 color(1.0f);
        for (int32 j = 0; j < maxBounces; j++) {
            if ((matIdx = getClosestHit(r, hit, scene)) >= 0) {
                ScatterResult res = scatterMaterial(&localRandState, r, hit, scene.materials[matIdx]);
                if (scene.materials[matIdx].type == core::EMMISIVE) {
                    color *= res.albedo;
                    totalColor += color;
                    break;
                }
                if (res.scattered) {
                    color *= res.albedo;
                    r = res.ray;
                } else {
                    break;
                }
            } else {
                // totalColor += color * vec3(0.0f);
                break;
            }
        }
    }
    totalColor /= samplesPerPixel;
    pixels[idx] = totalColor;

    randStates[idx] = localRandState;
}

} // namespace cuda
