#ifdef __APPLE__

#include "gpu/gpu_render.hpp"

// Std includes
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

// Local includes
#include "core/object.hpp"
#include "core/material.hpp"
#include "Forge/forge_c.h"
#include "core/scene.hpp"

namespace gpu {

static std::string loadSourceFromFile(std::string filepath) {
    std::fstream file(filepath, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Failed to open \"" << filepath << "\"\n";
        return std::string(1, (char) 0);
    }
    std::string src = "";
    std::string line;
    while (std::getline(file, line)) {
        src += line + "\n";
    }
    file.close();
    return src;
}

struct SceneData {
    ulong oCount, mCount;
};

GPURender::GPURender(GPURenderParams params) : params(params) {
    fgecInit();
}

std::vector<float3> GPURender::renderScene(const core::Scene& scene) {
    std::string kernelSrc = loadSourceFromFile("kernels/kernel.metal");
    if (kernelSrc[0] == 0) {
        return std::vector<float3>(0);
    }

    FGECShader* shader = fgecCreateShader();
    fgecShaderCompileLibrary(shader, kernelSrc.c_str());
    fgecShaderLoadKernel(shader, "render");

    fgecShaderAddBuffer(shader, &params, sizeof(GPURenderParams), 0, 0);

    SceneData sceneData;
    sceneData.oCount = scene.objects.size();
    sceneData.mCount = scene.materials.size();
    fgecShaderAddBuffer(shader, scene.objects.data(), sceneData.oCount * sizeof(core::Object), 0, 1);
    fgecShaderAddBuffer(shader, scene.materials.data(), sceneData.mCount * sizeof(core::Material), 0, 2);
    fgecShaderAddBuffer(shader, &sceneData, sizeof(SceneData), 0, 3);

    float3* outputBuffer = (float3*) fgecShaderCreateOutputBuffer(shader, params.screenW * params.screenH * sizeof(float3), 0, 4);

    ulong maxThreads = fgecShaderKernelGetMaxThreadsPerGroup(shader, "render");
    if (maxThreads > params.screenW * params.screenH) {
        maxThreads = params.screenW * params.screenH;
    }
    FGECSize groupSize = fgecSizeMake(
        std::sqrt(maxThreads),
        std::sqrt(maxThreads),
        1
    );
    FGECSize totalSize = fgecSizeMake(
        params.screenW,
        params.screenH,
        1
    );
    fgecShaderSetGroupSize(shader, groupSize);
    fgecShaderSetTotalSize(shader, totalSize);

    fgecShaderRunKernel(shader, "render");

    std::vector<float3> pixels(params.screenW * params.screenH);
    for (ulong i = 0; i < params.screenW * params.screenH; i++) {
        pixels[i] = outputBuffer[i];
    }
    return pixels;
}

} // namespace gpu

#endif // __APPLE__
