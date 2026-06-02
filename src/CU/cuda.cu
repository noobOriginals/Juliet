#include "CU/cuda.h"

// Std includes
#include <cstdlib>
#include <cuda_runtime.h>

// Local includes
#include "CU/cuda_vec3.cuh"
#include "CU/cuda_kernel.cuh"

static void* paramsBuffer; uint64 paramsSize;
static void* objectBuffer; uint64 objectSize;
static void* materialBuffer; uint64 materialSize;
static void* sceneDataBuffer; uint64 sceneDataSize;
static void* outputBuffer; uint64 outputSize;

static CUSize groupSize;
static CUSize gridSize;

static int32 ceilDiv(int32 a, int32 b) {
    return (a + b - 1) / b;
}

__host__ CUSize cuSizeMake(uint64 width, uint64 height, uint64 depth) {
    CUSize s;
    s.width = width;
    s.height = height;
    s.depth = depth;
    return s;
}

__host__ void cuSetParamsBuffer(const void* data, uint64 size) {
    cudaMalloc(&paramsBuffer, size);
    cudaMemcpy(paramsBuffer, data, size, cudaMemcpyDefault);
    paramsSize = size;
}

__host__ void cuSetObjectBuffer(const void* data, uint64 size) {
    cudaMalloc(&objectBuffer, size);
    cudaMemcpy(objectBuffer, data, size, cudaMemcpyDefault);
    objectSize = size;
}

__host__ void cuSetMaterialBuffer(const void* data, uint64 size) {
    cudaMalloc(&materialBuffer, size);
    cudaMemcpy(materialBuffer, data, size, cudaMemcpyDefault);
    materialSize = size;
}

__host__ void cuSetSceneDataBuffer(const void* data, uint64 size) {
    cudaMalloc(&sceneDataBuffer, size);
    cudaMemcpy(sceneDataBuffer, data, size, cudaMemcpyDefault);
    sceneDataSize = size;
}

__host__ const void* cuCreateOutputBuffer(uint64 size) {
    outputBuffer = malloc(size);
    outputSize = size;
    return outputBuffer;
}

__host__ void cuFreeMem() {
    cuFreeMemNoOutputBuffer();
    free(outputBuffer);
}

__host__ void cuFreeMemNoOutputBuffer() {
    cudaFree(paramsBuffer);
    cudaFree(objectBuffer);
    cudaFree(materialBuffer);
    cudaFree(sceneDataBuffer);
}

__host__ void cuSetGroupSize(CUSize group) {
    groupSize = group;
}

__host__ void cuSetTotalSize(CUSize total) {
    gridSize = cuSizeMake(
        ceilDiv(total.width, groupSize.width),
        ceilDiv(total.height, groupSize.height),
        ceilDiv(total.depth, groupSize.depth)
    );
}

__host__ void cuRunKernel() {
    void* cudaOutBuffer;
    cudaMalloc(&cudaOutBuffer, outputSize);
    dim3 group(groupSize.width, groupSize.height);
    dim3 grid(gridSize.width, gridSize.height);
    cu::kernel<<<grid, group>>>(
        (const cu::RenderParams*) paramsBuffer,
        (const cu::Object*) objectBuffer,
        (const cu::Material*) materialBuffer,
        (const cu::SceneData*) sceneDataBuffer,
        (cu::vec3*) cudaOutBuffer
    );
    cudaDeviceSynchronize();
    cudaMemcpy(outputBuffer, cudaOutBuffer, outputSize, cudaMemcpyDefault);
}
