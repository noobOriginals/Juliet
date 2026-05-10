// Std includes
#include <iostream>

// Lib includes
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Local includes
#include <util/types.h>
#include <util/random.hpp>
#include <util/util.hpp>
#include <lib/render.hpp>
#include <core/scene.hpp>

using namespace glm;
using namespace util;

core::Scene scn;

vec3 skyColor(const core::Ray& ray) {
    // float32 a = 0.5f * (ray.dir.y + 1.0f);
    // return (1.0f - a) * vec3(1.0f) + a * vec3(0.5f, 0.7f, 1.0f);
    return vec3(0.0f);
}

vec3 raytrace(const core::Ray& ray, int32 maxDepth, PCG32& rng) {
    core::Ray r = ray;
    core::HitRecord h;
    int32 mIdx;
    vec3 color = vec3(1.0f);
    for (int32 i = 0; i < maxDepth; i++) {
        if ((mIdx = core::getClosestHit(r, h, scn)) >= 0) {
            core::ScatterResult res = core::scatterMaterial(r, h, rng, scn.materials[mIdx]);
            if (scn.materials[mIdx].type == core::EMMISIVE) {
                color *= res.albedo;
                return color;
            }
            if (res.scattered) {
                color *= res.albedo;
                r = res.ray;
            } else {
                return vec3();
            }
        } else {
            return color * skyColor(r);
        }
    }
    return vec3();
}

#define aabbData(r, c) c - vec3(r * 0.5f), c + vec3(r * 0.5f)
#define obbData(r, c, rot) c - vec3(r * 0.5f), c + vec3(r * 0.5f), vec3(glm::rotate(glm::mat4(1.0f), (float32)util::degToRad(rot), vec3(0, 1, 0)) * vec4(1.0f, 0.0f, 0.0f, 1.0f)), vec3(0.0f, 1.0f, 0.0f)

int main() {
    core::addObjectToScene(core::makeQuad(vec3(0.0f, -0.5f, 0.0f), vec3(10.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 10.0f), 0), scn);
    core::addObjectToScene(core::makeAABB(aabbData(1.0f, vec3(-1.0f, 0.0f, -3.0f)), 3), scn);
    core::addObjectToScene(core::makeAABB(aabbData(1.0f, vec3(3.0f, 0.0f, 0.5f)), 3), scn);
    core::addObjectToScene(core::makeOBB(obbData(1.0f, vec3(2.0f, 0.0f, -2.5f), 45.0f), 3), scn);
    core::addObjectToScene(core::makeOBB(obbData(1.0f, vec3(0.5f, 2.0f, 0.75f), 15.0f), 3), scn);
    core::addObjectToScene(core::makeSphere(vec3(0.5f, 0.0f, -0.5f), 0.5f, 1), scn);
    core::addObjectToScene(core::makeSphere(vec3(-0.5f, 0.0f, 0.5f), 0.5f, 2), scn);
    core::addObjectToScene(core::makeOBB(vec3(-1.5f, -0.5f, -1.5f), vec3(-0.5f, 0.5f, -0.5f), vec3(1.0f, 0.0f, -0.2f), vec3(0.0f, 1.0f, 0.0f), 4), scn);
    core::addObjectToScene(core::makeOBB(obbData(1.0f, vec3(0.7f, 0.0f, 1.1f), -15.0f), 5), scn);
    core::addObjectToScene(core::makeQuad(vec3(0.0f, 4.5f, -5.0f), vec3(10.0f, 0.0f, 0.0f), vec3(0.0f, -10.0f, 0.0f), 6), scn);
    core::addObjectToScene(core::makeQuad(vec3(5.0f, 4.5f, 0.0f), vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, -10.0f, 0.0f), 6), scn);

    core::addMaterialToScene(core::makeDiffuse(vec3(0.196f, 0.541f, 0.0f)), scn);
    core::addMaterialToScene(core::makeDiffuse(vec3(0.922f, 0.561f, 0.082f)), scn);
    core::addMaterialToScene(core::makeDielectric(vec3(0.9f, 0.9f, 0.9f), 1.5f), scn);
    core::addMaterialToScene(core::makeEmmisive(vec3(0.9f, 0.9f, 0.9f)), scn);
    core::addMaterialToScene(core::makeDielectric(vec3(0.871, 1, 0.467), 1.5f), scn);
    core::addMaterialToScene(core::makeMetal(vec3(0.8, 0.9, 1.0), 0.05f), scn);
    core::addMaterialToScene(core::makeMetal(vec3(1.0, 1.0, 1.0), 0.02f), scn);

    lib::RenderParameters renderParameters;

    renderParameters.screenWidth = 2560;
    renderParameters.screenHeight = 1440;
    renderParameters.vfov = 32.0f;

    renderParameters.worldUp = vec3(0, 1, 0);
    renderParameters.cameraPos = vec3(-8.0f, 2.8f, 3.8f);
    renderParameters.cameraLookAt = vec3(0.0f, 0.5f, -0.5f);

    renderParameters.samplesPerPixel = 1000;
    renderParameters.maxBounces = 500;

    renderParameters.enableSupersampling = true;
    renderParameters.enableGammaCorrection = true;
    renderParameters.enableMultiThreading = true;

    renderParameters.raytraceCallback = raytrace;

    lib::Render render(renderParameters);
    render.render();
    render.save("renders/render.png");

    return 0;
}
