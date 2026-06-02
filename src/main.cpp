// Std includes
// #include <iostream>

// Lib includes
#include "core/material.hpp"
#include "core/object.hpp"
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

    int32 midx;
    core::ScatterResult sres;

    vec3 throughput(1.0f);
    vec3 radiance(0.0f);

    for (int32 i = 0; i < maxDepth; i++) {
        midx = core::getClosestHit(r, h, scene);

        if (midx < 0) {
            break;
        }

        vec3 emission(0.0f);
        sres = core::scatterMaterial(r, h, scene.materials[midx]);
        if (scene.materials[midx].type == core::EMISSIVE) {
            emission = sres.albedo;
        }

        if (!sres.scattered) {
            break;
        }

        throughput *= sres.albedo;
        radiance += emission * throughput;

        r = sres.ray;
    }

    return radiance;
}

#define aabbData(r, c) c - vec3(r * 0.5f), c + vec3(r * 0.5f)
#define obbData(r, c, rot) c - r, c + vec3(r * 0.5f), vec3(rotate(mat4(1.0f), (float32) util::degToRad(rot), vec3(0.2, 1, 0.5)) * vec4(1.0f, 0.0f, 0.0f, 1.0f)), vec3(0.0f, 1.0f, 0.0f)
#define obbData2(r, c, rot) c - r, c + vec3(r * 0.5f), vec3(rotate(mat4(1.0f), (float32) util::degToRad(rot), normalize(vec3(1, 1, 1))) * vec4(1.0f, 0.0f, 0.0f, 1.0f)), vec3(0.0f, 1.0f, 0.0f)

int main() {
    core::Scene scn = core::loadSceneFromFile("scenes/cornell.scn");

    if (!scn.isValid) {
        return 1;
    }

    scn.objects.push_back(core::makeOBB(obbData(vec3(2.0f, 3.5f, 2.0f), vec3(-2.0f, -1.0f, 0.5f), 25.0f), 5));
    scn.objects.push_back(core::makeSphere(vec3(2.0f, -3.0f, -1.0f), 2.0f, 6));
    scn.objects.push_back(core::makeOBB(obbData2(vec3(1.5f, 1.5f, 1.5f), vec3(3.0f, 2.0f, 0.0f), -40.0f), 7));
    scn.objects.push_back(core::makeSphere(vec3(-3.0f, 2.7f, 1.0f), 1.5f, 8));
    scn.objects.push_back(core::makeSphere(vec3(3.8f, -4.4f, 1.6f), 0.6f, 9));

    scn.materials.push_back(core::makeDielectric(vec3(1.0f), 1.5f, 0.0f));
    scn.materials.push_back(core::makeMetal(vec3(0.5f, 0.5f, 0.5f), 0.0f, 0.2f));
    scn.materials.push_back(core::makeDielectric(vec3(1.0f), 1.5f, 0.5f));
    scn.materials.push_back(core::makeDielectric(vec3(1.0f), 1.5f, 0.15f));
    scn.materials.push_back(core::makeDiffuse(vec3(0.808f, 0.929f, 0.149f)));

    lib::RenderParameters renderParameters;

    renderParameters.screenWidth = 1000;
    renderParameters.screenHeight = 1000;
    renderParameters.vfov = 25.0f;

    renderParameters.worldUp = vec3(0, 1, 0);
    renderParameters.cameraPos = vec3(0.0f, 0.0f, 30.0f);
    renderParameters.cameraLookAt = vec3(0.0f, 0.0f, 0.0f);

    renderParameters.samplesPerPixel = 1000;
    renderParameters.maxBounces = 100;
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
