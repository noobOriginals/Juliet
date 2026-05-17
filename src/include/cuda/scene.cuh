#ifndef CUDA_SCENE_CUH
#define CUDA_SCENE_CUH

// Std includes
#include <cuda_runtime.h>

// Local includes
#include <util/types.h>
#include <cuda/object.cuh>
#include <cuda/material.cuh>

namespace cuda {

struct Scene {
    Object* objects;
    uint64 objCount;
    Material* materials;
    uint64 matCount;
};

inline __device__ int32 getClosestHit(const Ray& ray, HitRecord& hit, const Scene& scene) {
    int32 matIdx = -1;
    float32 closestT = 3.4028235e+38f;
    for (uint64 i = 0; i < scene.objCount; i++) {
        if (hitObject(ray, hit, 1e-4f, closestT, scene.objects[i])) {
            closestT = hit.t;
            matIdx = scene.objects[i].materialIdx;
        }
    }
    return matIdx;
}

} // namespace cuda

#endif // CUDA_SCENE_CUH
