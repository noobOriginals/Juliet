#ifndef CUDA_RANDOM_INIT_KERNELS_CUH
#define CUDA_RANDOM_INIT_KERNELS_CUH

// Std includes
#include <cuda_runtime.h>
#include <curand_kernel.h>

// Local includes
#include <util/types.h>

namespace cuda {

__global__ void initStatesKernel1D(curandStatePhilox4_32_10* states, uint64 numStates);
__global__ void initStatesKernel2D(curandStatePhilox4_32_10* states, uint64 numStates);
__global__ void initStatesKernel3D(curandStatePhilox4_32_10* states, uint64 numStates);

} // namespace cuda

#endif // CUDA_RANDOM_INIT_KERNELS_CUH
