#ifndef CUDA_RAY_CUH
#define CUDA_RAY_CUH

// Std includes
#include <cuda_runtime.h>

// Local includes
#include <util/types.h>
#include <cuda/vec3.cuh>

namespace cuda {

struct Ray {
    vec3 org, dir;
};

inline __device__ vec3 rayAt(const Ray& r, float32 t) {
    return r.org + r.dir * t;
}

} // namespace cuda

#endif // CUDA_RAY_CUH