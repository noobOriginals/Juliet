#ifndef CUDA_OBJECT_CUH
#define CUDA_OBJECT_CUH

// Std includes
#include <cuda_runtime.h>

// Local includes
#include <util/types.h>
#include <core/object_properties.hpp>
#include <cuda/vec3.cuh>
#include <cuda/ray.cuh>
#include <cuda/hitrecord.cuh>

namespace cuda {

struct Object {
    int32 type;
    float32 data[core::OBJECT_DATA_SIZE];
    int32 materialIdx;
};

__device__ bool hitSphere(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[core::OBJECT_DATA_SIZE]);
__device__ bool hitTriangle(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[core::OBJECT_DATA_SIZE]);
__device__ bool hitQuad(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[core::OBJECT_DATA_SIZE]);
__device__ bool hitAABB(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[core::OBJECT_DATA_SIZE]);
__device__ bool hitOBB(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[core::OBJECT_DATA_SIZE]);
__device__ bool hitObject(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const Object& obj);

} // namespace cuda

#endif  // CUDA_OBJECT_CUH