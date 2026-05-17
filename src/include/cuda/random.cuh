#ifndef CUDA_RANDOM_CUH
#define CUDA_RANDOM_CUH

// Std includes
#include <cuda_runtime.h>
#include <curand_kernel.h>

// Local includes
#include <util/types.h>
#include <util/epsilon.h>
#include <cuda/vec3.cuh>

namespace cuda {

inline __device__ void initState(curandStatePhilox4_32_10* state, uint64 seed) {
    curand_init(seed, 12345, 0, state);
}

inline __device__ float32 nextFloat(curandStatePhilox4_32_10* state) {
    return curand_uniform(state);
}

inline __device__ float32 nextGaussian(curandStatePhilox4_32_10* state) {
    return curand_normal(state);
}

inline __device__ vec3 randomVec3(curandStatePhilox4_32_10* state) {
    return vec3(nextFloat(state), nextFloat(state), nextFloat(state));
}

inline __device__ vec3 randomUV(curandStatePhilox4_32_10* state) {
    return vec3(nextGaussian(state), nextGaussian(state), nextGaussian(state));
}

inline __device__ vec3 randomHemisphere(curandStatePhilox4_32_10* state, const vec3& normal) {
    vec3 v = randomUV(state);
    return (dot(normal, v) < 0.0f) ? -v : v;
}

inline __device__ vec3 randomCosineHemisphere(curandStatePhilox4_32_10* state, const vec3& normal) {
    vec3 v = normal + randomUV(state);
    if (lenSq(v) <= EPSILON * EPSILON) {
        return normal;
    }
    return v;
}

} // namespace cuda

#endif // CUDA_RANDOM_CUH