#include <cuda/render_interface.hpp>

// Std includes
#include <cuda_runtime.h>
#include <curand_kernel.h>

// Local includes
#include <cuda/vec3.cuh>
#include <cuda/object.cuh>
#include <cuda/material.cuh>
#include <cuda/scene.cuh>
#include <cuda/random_init_kernels.cuh>
#include <cuda/kernel.cuh>

namespace cuda {

namespace interface {

__host__ cuda::vec3 interfaceToVec3(const vec3& v) {
    return cuda::vec3(v.x, v.y, v.z);
}

__host__ vec3 vec3ToInterface(const cuda::vec3& v) {
    return {v.x, v.y, v.z};
}

__host__ uint32 ceil_div(uint32 a, uint32 b) {
    return (a + b - 1) / b;
}

__host__ void renderData(RenderReturnData* retData, const RenderData* renderData) {
    uint64 numPixels = renderData->screenW * renderData->screenH;
    curandStatePhilox4_32_10* states;
    cudaMalloc(&states, sizeof(curandStatePhilox4_32_10) * numPixels);
    dim3 blockSize(16, 16);
    dim3 gridSize(ceil_div(renderData->screenW, 16), ceil_div(renderData->screenH, 16));
    initStatesKernel2D<<<gridSize, blockSize>>>(states, numPixels);
    cuda::vec3* pixels;
    cudaMalloc(&pixels, sizeof(cuda::vec3) * numPixels);
    cuda::Scene scene;
    scene.objCount = renderData->objCount;
    scene.matCount = renderData->matCount;
    cudaMalloc(&scene.objects, sizeof(cuda::Object) * scene.objCount);
    cudaMalloc(&scene.materials, sizeof(cuda::Material) * scene.matCount);
    cudaMemcpy(scene.objects, renderData->objects, sizeof(cuda::Object) * scene.objCount, cudaMemcpyDefault);
    cudaMemcpy(scene.materials, renderData->materials, sizeof(cuda::Material) * scene.matCount, cudaMemcpyDefault);
    renderScene<<<gridSize, blockSize>>>(
        states,
        numPixels,
        pixels,
        renderData->screenW,
        renderData->screenH,
        interfaceToVec3(renderData->camPos),
        interfaceToVec3(renderData->pixelOrigin),
        interfaceToVec3(renderData->pixelDeltaW),
        interfaceToVec3(renderData->pixelDeltaH),
        renderData->maxBounces,
        renderData->samplesPerPixel,
        scene
    );
    cudaDeviceSynchronize();
    cudaFree(scene.materials);
    cudaFree(scene.objects);
    cudaMallocHost(&retData->pixels, sizeof(vec3) * numPixels);
    cudaMemcpy(retData->pixels, pixels, sizeof(vec3) * numPixels, cudaMemcpyDefault);
    retData->numPixels = numPixels;
    cudaFree(pixels);
    cudaFree(states);
}

void cudaFreeMem(void* ptr) {
    cudaFree(ptr);
}

} // namespace interface

} // namespace cuda
