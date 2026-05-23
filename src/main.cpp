// Std includes
// #include <iostream>

// Lib includes
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Local includes
#include <util/types.h>
#include <util/util.hpp>
#include <lib/render.hpp>
#include <core/scene.hpp>

using namespace glm;
using namespace util;

vec3 skyColor(const core::Ray& ray) {
    // float32 a = 0.5f * (ray.dir.y + 1.0f);
    // return (1.0f - a) * vec3(1.0f) + a * vec3(0.5f, 0.7f, 1.0f);
    return vec3(0.0f);
}

vec3 raytrace(const core::Scene& scene, const core::Ray& ray, int32 maxDepth) {
    core::Ray r = ray;
    core::HitRecord h;
    int32 mIdx;
    vec3 color = vec3(1.0f);
    for (int32 i = 0; i < maxDepth; i++) {
        if ((mIdx = core::getClosestHit(r, h, scene)) >= 0) {
            core::ScatterResult res = core::scatterMaterial(r, h, scene.materials[mIdx]);
            color *= res.albedo;
            if (!res.scattered) {
                return color;
            }
            r = res.ray;
        } else {
            return color * skyColor(r);
        }
    }
    return vec3();
}

#define aabbData(r, c) c - vec3(r * 0.5f), c + vec3(r * 0.5f)
#define obbData(r, c, rot) c - vec3(r * 0.5f), c + vec3(r * 0.5f), vec3(glm::rotate(glm::mat4(1.0f), (float32) util::degToRad(rot), vec3(0, 1, 0)) * vec4(1.0f, 0.0f, 0.0f, 1.0f)), vec3(0.0f, 1.0f, 0.0f)

int main() {
    core::Scene scn = core::loadSceneFromFile("scenes/cornell.scn");
    if (!scn.isValid) {
        return 1;
    }

    lib::RenderParameters renderParameters;

    renderParameters.screenWidth = 1440;
    renderParameters.screenHeight = 1440;
    renderParameters.vfov = 25.0f;

    renderParameters.worldUp = vec3(0, 1, 0);
    renderParameters.cameraPos = vec3(0.0f, 0.0f, 30.0f);
    renderParameters.cameraLookAt = vec3(0.0f, 0.0f, 0.0f);

    renderParameters.samplesPerPixel = 10000;
    renderParameters.maxBounces = 50;
    renderParameters.threadTileSize = 32;

    renderParameters.enableSupersampling = true;
    renderParameters.enableGammaCorrection = true;
    renderParameters.enableMultiThreading = true;
    renderParameters.useGPU = true;

    renderParameters.raytraceCallback = raytrace;

    lib::Render render(renderParameters);
    render.renderScene(scn);
    render.save("renders/render.png");

    return 0;
}
