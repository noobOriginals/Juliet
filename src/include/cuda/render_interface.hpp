#ifndef CUDA_RENDER_INTERFACE_HPP
#define CUDA_RENDER_INTERFACE_HPP

// Local includes
#include <util/types.h>
#include <core/object_properties.hpp>
#include <core/material_properties.hpp>

namespace cuda {

namespace interface {

struct vec3 {
    float32 x, y, z;
};

struct Object {
    int32 type;
    float32 data[core::OBJECT_DATA_SIZE];
    int32 matIdx;
};

struct Material {
    int32 type;
    float32 data[core::MATERIAL_DATA_SIZE];
};

struct RenderData {
    int32 screenW, screenH;
    vec3 camPos, pixelOrigin, pixelDeltaW, pixelDeltaH;
    int32 maxBounces, samplesPerPixel;
    Object* objects;
    uint64 objCount;
    Material* materials;
    uint64 matCount;
};

struct RenderReturnData {
    vec3* pixels;
    int32 numPixels;
};

void renderData(RenderReturnData* retData, const RenderData* renderData);

} // namespace interface

} // namespace cuda

#endif // CUDA_RENDER_INTERFACE_HPP
