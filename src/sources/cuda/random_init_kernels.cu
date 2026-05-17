#include <cuda/random_init_kernels.cuh>

// Local includes
#include <cuda/random.cuh>

namespace cuda {

__global__ void initStatesKernel1D(curandStatePhilox4_32_10* states, uint64 numStates) {
    uint64 idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < numStates) {
        initState(&states[idx], idx);
    }
}

__global__ void initStatesKernel2D(curandStatePhilox4_32_10* states, uint64 numStates) {
    uint64 idx = (blockIdx.y * blockDim.y + threadIdx.y) * gridDim.x * blockDim.x +  blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < numStates) {
        initState(&states[idx], idx);
    }
}

__global__ void initStatesKernel3D(curandStatePhilox4_32_10* states, uint64 numStates) {
    uint64 idx = (blockIdx.z * blockDim.z + threadIdx.z) * gridDim.x * blockDim.x * gridDim.y * blockDim.y + (blockIdx.y * blockDim.y + threadIdx.y) * gridDim.x * blockDim.x +  blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < numStates) {
        initState(&states[idx], idx);
    }
}

} // namespace cuda
