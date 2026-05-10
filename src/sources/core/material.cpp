#include <core/material.hpp>

// Std includes
#include <cmath>

// Local includes
#include <util/epsilon.h>
#include <util/util.hpp>

namespace core {

Material makeDiffuse(const glm::vec3& albedo) {
    Material mat;
    mat.type = DIFFUSE;
    mat.data[0] = albedo.x;
    mat.data[1] = albedo.y;
    mat.data[2] = albedo.z;
    return mat;
}

Material makeMetal(const glm::vec3& albedo, float32 fuzz) {
    Material mat;
    mat.type = METAL;
    mat.data[0] = albedo.x;
    mat.data[1] = albedo.y;
    mat.data[2] = albedo.z;
    mat.data[3] = fuzz;
    return mat;
}

Material makeDielectric(const glm::vec3& albedo, float32 refIdx) {
    Material mat;
    mat.type = DIELECTRIC;
    mat.data[0] = albedo.x;
    mat.data[1] = albedo.y;
    mat.data[2] = albedo.z;
    mat.data[3] = refIdx;
    return mat;
}

Material makeEmmisive(const glm::vec3& albedo) {
    Material mat;
    mat.type = EMMISIVE;
    mat.data[0] = albedo.x;
    mat.data[1] = albedo.y;
    mat.data[2] = albedo.z;
    return mat;
}

ScatterResult scatterDiffuse(const Ray& ray, const HitRecord& hit, util::PCG32& rng, const float32 data[MATERIAL_DATA_SIZE]) {
    ScatterResult res;
    res.albedo = glm::vec3(data[0], data[1], data[2]);
    res.scattered = true;
    res.ray.org = hit.p;
    res.ray.dir = util::diffuse(hit.n);
    return res;
}

ScatterResult scatterMetal(const Ray& ray, const HitRecord& hit, util::PCG32& rng, const float32 data[MATERIAL_DATA_SIZE]) {
    ScatterResult res;
    res.albedo = glm::vec3(data[0], data[1], data[2]);
    res.scattered = true;
    res.ray.org = hit.p;
    res.ray.dir = util::reflect(ray.dir, hit.n) + util::randomUV() * data[3];
    return res;
}

ScatterResult scatterDielectric(const Ray& ray, const HitRecord& hit, util::PCG32& rng, const float32 data[MATERIAL_DATA_SIZE]) {
    ScatterResult res;
    res.albedo = glm::vec3(data[0], data[1], data[2]);
    res.scattered = true;
    res.ray.org = hit.p;
    float32 n1 = 1.0f, n2 = data[3];
    if (hit.exit) {
        float32 tmp = n1;
        n1 = n2;
        n2 = tmp;
    }
    res.ray.dir = util::refract(ray.dir, hit.n, n1, n2);
    return res;
}

ScatterResult scatterEmmisive(const Ray& ray, const HitRecord& hit, util::PCG32& rng, const float32 data[MATERIAL_DATA_SIZE]) {
    return scatterDiffuse(ray, hit, rng, data);
}

ScatterResult scatterMaterial(const Ray& ray, const HitRecord& hit, util::PCG32& rng, const Material& material) {
    switch (material.type) {
    case DIFFUSE: {
        return scatterDiffuse(ray, hit, rng, material.data);
    }

    case METAL: {
        return scatterMetal(ray, hit, rng, material.data);
    }

    case DIELECTRIC: {
        return scatterDielectric(ray, hit, rng, material.data);
    }

    case EMMISIVE: {
        return scatterEmmisive(ray, hit, rng, material.data);
    }

    default: {
        ScatterResult res;
        res.scattered = false;
        return res;
    }
    }
}


}  // namespace core