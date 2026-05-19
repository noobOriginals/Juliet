#ifndef CUDA_HITRECORD_HPP
#define CUDA_HITRECORD_HPP

// Std includes
#include <cuda_runtime.h>

// Local includes
#include <util/types.h>
#include <cuda/vec3.cuh>
#include <cuda/ray.cuh>

namespace cuda {

struct HitRecord {
    float32 t;
    vec3 p, n;
    bool exit;
};

inline __device__ void setHitNormal(HitRecord& hr, const Ray& r, const vec3& normal) {
    if (dot(r.dir, normal) > 0) {
        hr.n = -normal;
        hr.exit = true;
    } else {
        hr.n = normal;
        hr.exit = false;
    }
}

}  // namespace cuda

#endif  // CUDA_HITRECORD_HPP