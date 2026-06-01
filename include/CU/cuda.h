#ifndef CU_CUDA_H
#define CU_CUDA_H

#include "util/types.h"

typedef struct {
    uint64 width, height, depth;
} CUSize;

CUSize cuSizeMake(uint64 width, uint64 height, uint64 depth);

void cuSetParamsBuffer(const void* data, uint64 size);
void cuSetObjectBuffer(const void* data, uint64 size);
void cuSetMaterialBuffer(const void* data, uint64 size);
void cuSetSceneDataBuffer(const void* data, uint64 size);
const void* cuCreateOutputBuffer(uint64 size);
void cuFreeMem();
void cuFreeMemNoOutputBuffer();
void cuSetGroupSize(CUSize groupSize);
void cuSetTotalSize(CUSize totalSize);
void cuRunKernel();

#endif // CU_CUDA_H
