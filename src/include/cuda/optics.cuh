#ifndef CUDA_OPTICS_CUH
#define CUDA_OPTICS_CUH

// Std includes
#include <cuda_runtime.h>
#include <curand_kernel.h>

// Local includes
#include <util/types.h>
#include <util/epsilon.h>
#include <cuda/vec3.cuh>
#include <cuda/random.cuh>

namespace cuda {

// Returns a random unit vector sampled on the cosine hemisphere around the normal
inline __device__ vec3 diffuse(curandStatePhilox4_32_10* state, const vec3& normal) {
    return normalize(randomCosineHemisphere(state, normal));
}

// Returns a reflected unit vector around the normal. If fuzz > 0.0f, introduces a cosine hemisphere sampled fuzz vector.
inline __device__ vec3 reflect(curandStatePhilox4_32_10* state, const vec3& v, const vec3& normal, float32 fuzz) {
    vec3 ref = v - 2.0f * dot(v, normal) * normal;
    if (fuzz == 0.0f) {
        return ref;
    }
    vec3 fuzzed = ref + randomUV(state) * fuzz;
    if (lenSq(fuzzed) <= EPSILON * EPSILON) {
        return ref;
    }
    return normalize(fuzzed);
}

// Schlick (Fresnel) approximation
inline __device__ float32 reflectance(float32 cos, float32 n1, float32 n2) {
    float32 r0 = (n1 - n2) / (n1 + n2);
    r0 *= r0;
    return r0 + (1.0f - r0) * pow((1.0f - cos), 5);
}

// Returns the refracted/reflected unit vector (depending on TIR or Schlick)
inline __device__ vec3 refract(curandStatePhilox4_32_10* state, const vec3& dir, const vec3& normal, float32 n1, float32 n2) {
    float32 cos = dot(-dir, normal);
    float32 sin = sqrt(max(0.0f, 1.0f - cos * cos));
    float32 idx = n1 / n2;
    if (sin * idx > 1.0f || reflectance(cos, n1, n2) > nextGaussian(state)) {
        return dir + 2.0f * cos * normal;
    }
    vec3 perp = idx * (dir + cos * normal);
    vec3 para = -sqrt(fabs(1.0f - dot(perp, perp))) * normal;
    return perp + para;
}

} // namespace cuda

#endif // CUDA_OPTICS_CUH