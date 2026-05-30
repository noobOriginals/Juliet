#include "core/cpu/material_ops.hpp"

// Local includes
#include "util/optics.hpp"

namespace core {

void makeDiffuse(float32 data[MATERIAL_DATA_SIZE], const glm::vec3 albedo) {
    data[0] = albedo.x;
    data[1] = albedo.y;
    data[2] = albedo.z;
    data[3] = 0.0f;
    data[4] = 0.0f;
    data[5] = 0.0f;
    data[6] = 0.0f;
}

void makeMetal(float32 data[MATERIAL_DATA_SIZE], const glm::vec3 albedo, float32 fuzz) {
    data[0] = albedo.x;
    data[1] = albedo.y;
    data[2] = albedo.z;
    data[3] = 0.0f;
    data[4] = 0.0f;
    data[5] = 0.0f;
    data[6] = fuzz;
}

void makeDielectric(float32 data[MATERIAL_DATA_SIZE], const glm::vec3 albedo, float32 refIdx) {
    data[0] = albedo.x;
    data[1] = albedo.y;
    data[2] = albedo.z;
    data[3] = 0.0f;
    data[4] = 0.0f;
    data[5] = 0.0f;
    data[6] = refIdx;
}

void makeEmissiveDiffuse(float32 data[MATERIAL_DATA_SIZE], const glm::vec3 albedo, const glm::vec3 emission) {
    data[0] = albedo.x;
    data[1] = albedo.y;
    data[2] = albedo.z;
    data[3] = emission.x;
    data[4] = emission.y;
    data[5] = emission.z;
    data[6] = 0.0f;
}

void makeEmissiveMetal(float32 data[MATERIAL_DATA_SIZE], const glm::vec3 albedo, float32 fuzz, const glm::vec3 emission) {
    data[0] = albedo.x;
    data[1] = albedo.y;
    data[2] = albedo.z;
    data[3] = emission.x;
    data[4] = emission.y;
    data[5] = emission.z;
    data[6] = 0.0f;
}

void makeEmissiveDielectric(float32 data[MATERIAL_DATA_SIZE], const glm::vec3 albedo, float32 refIdx, const glm::vec3 emission) {
    data[0] = albedo.x;
    data[1] = albedo.y;
    data[2] = albedo.z;
    data[3] = emission.x;
    data[4] = emission.y;
    data[5] = emission.z;
    data[6] = 0.0f;
}

ScatterResult scatterDiffuse(float32 data[MATERIAL_DATA_SIZE], util::PCG32& rng, const Ray& ray, const HitRecord& hit) {
    ScatterResult res;
    res.albedo = glm::vec3(data[0], data[1], data[2]);
    res.emission = glm::vec3(data[3], data[4], data[5]);
    res.valid = true;
    res.ray.dir = util::diffuse(rng, hit.n);
    res.ray.org = hit.p + res.ray.dir * 1e-4f;
    return res;
}

ScatterResult scatterMetal(float32 data[MATERIAL_DATA_SIZE], util::PCG32& rng, const Ray& ray, const HitRecord& hit) {
    ScatterResult res;
    res.albedo = glm::vec3(data[0], data[1], data[2]);
    res.emission = glm::vec3(data[3], data[4], data[5]);
    res.valid = true;
    res.ray.dir = glm::normalize(util::reflect(rng, ray.dir, hit.n) + util::randomUV(rng) * data[6]);
    res.ray.org = hit.p + res.ray.dir * 1e-4f;
    return res;
}

ScatterResult scatterDielectric(float32 data[MATERIAL_DATA_SIZE], util::PCG32& rng, const Ray& ray, const HitRecord& hit) {
    ScatterResult res;
    res.albedo = glm::vec3(data[0], data[1], data[2]);
    res.emission = glm::vec3(data[3], data[4], data[5]);
    res.valid = true;
    float32 n1 = 1.0f, n2 = data[6];
    if (hit.exit) {
        float32 tmp = n1;
        n1 = n2;
        n2 = tmp;
    }
    res.ray.dir = glm::normalize(util::refract(rng, ray.dir, hit.n, n1, n2));
    res.ray.org = hit.p + res.ray.dir * 1e-4f;
    return res;
}

ScatterResult scatterMaterial(int32 type, float32 data[MATERIAL_DATA_SIZE], util::PCG32& rng, const Ray& ray, const HitRecord& hit) {
    switch (type) {
    case DIFFUSE: return scatterDiffuse(data, rng, ray, hit);
    case METAL: return scatterMetal(data, rng, ray, hit);
    case DIELECTRIC: return scatterDielectric(data, rng, ray, hit);
    default: return ScatterResult{};
    }
}

} // namespace core
