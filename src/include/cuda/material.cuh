#ifndef CUDA_MATERIAL_CUH
#define CUDA_MATERIAL_CUH

// Std includes
#include <cuda_runtime.h>
#include <curand_kernel.h>

// Local includes
#include <util/types.h>
#include <core/material_properties.hpp>
#include <cuda/vec3.cuh>
#include <cuda/ray.cuh>
#include <cuda/hitrecord.cuh>

namespace cuda {

struct Material {
    int32 type;
    float32 data[core::MATERIAL_DATA_SIZE];
};

struct ScatterResult {
    Ray ray;
    vec3 albedo;
    bool scattered;
};

__device__ ScatterResult scatterDiffuse(curandStatePhilox4_32_10* randState, const Ray& ray, const HitRecord& hit, const float32 data[core::MATERIAL_DATA_SIZE]);
__device__ ScatterResult scatterMetal(curandStatePhilox4_32_10* randState, const Ray& ray, const HitRecord& hit, const float32 data[core::MATERIAL_DATA_SIZE]);
__device__ ScatterResult scatterDielectric(curandStatePhilox4_32_10* randState, const Ray& ray, const HitRecord& hit, const float32 data[core::MATERIAL_DATA_SIZE]);
__device__ ScatterResult scatterEmmisive(curandStatePhilox4_32_10* randState, const Ray& ray, const HitRecord& hit, const float32 data[core::MATERIAL_DATA_SIZE]);
__device__ ScatterResult scatterMaterial(curandStatePhilox4_32_10* randState, const Ray& ray, const HitRecord& hit, const Material& material);

} // namespace cuda

#endif // CUDA_MATERIAL_CUH
