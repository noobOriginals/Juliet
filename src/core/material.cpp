#include <core/material.hpp>

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
    mat.data[3] = 0.0f;
    mat.data[4] = 0.0f;
    return mat;
}

Material makeMetal(const glm::vec3& albedo, float32 fuzz, float32 metallic) {
    Material mat;
    mat.type = METAL;
    mat.data[0] = albedo.x;
    mat.data[1] = albedo.y;
    mat.data[2] = albedo.z;
    mat.data[3] = fuzz;
    mat.data[4] = metallic;
    return mat;
}

Material makeDielectric(const glm::vec3& albedo, float32 refIdx, float32 frost) {
    Material mat;
    mat.type = DIELECTRIC;
    mat.data[0] = albedo.x;
    mat.data[1] = albedo.y;
    mat.data[2] = albedo.z;
    mat.data[3] = refIdx;
    mat.data[4] = frost;
    return mat;
}

Material makeEmissive(const glm::vec3& albedo) {
    Material mat;
    mat.type = EMISSIVE;
    mat.data[0] = albedo.x;
    mat.data[1] = albedo.y;
    mat.data[2] = albedo.z;
    mat.data[3] = 0.0f;
    mat.data[4] = 0.0f;
    return mat;
}

ScatterResult scatterDiffuse(const Ray& ray, const HitRecord& hit, const float32 data[MATERIAL_DATA_SIZE]) {
    ScatterResult res;
    res.albedo = glm::vec3(data[0], data[1], data[2]);
    res.scattered = true;
    res.ray.org = hit.p;
    res.ray.dir = util::diffuse(hit.n);
    return res;
}

ScatterResult scatterMetal(const Ray& ray, const HitRecord& hit, const float32 data[MATERIAL_DATA_SIZE]) {
    ScatterResult res;
    res.albedo = glm::vec3(data[0], data[1], data[2]);
    res.scattered = true;
    res.ray.org = hit.p;
    res.ray.dir = glm::normalize(util::reflect(ray.dir, hit.n) + util::randomUV() * data[3]);
    return res;
}

ScatterResult scatterDielectric(const Ray& ray, const HitRecord& hit, const float32 data[MATERIAL_DATA_SIZE]) {
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
    res.ray.dir = glm::normalize(util::refract(ray.dir, hit.n, n1, n2));
    return res;
}

ScatterResult scatterEmissive(const Ray& ray, const HitRecord& hit, const float32 data[MATERIAL_DATA_SIZE]) {
    return scatterDiffuse(ray, hit, data);
}

ScatterResult scatterMaterial(const Ray& ray, const HitRecord& hit, const Material& material) {
    switch (material.type) {
    case DIFFUSE: {
        return scatterDiffuse(ray, hit, material.data);
    }

    case METAL: {
        return scatterMetal(ray, hit, material.data);
    }

    case DIELECTRIC: {
        return scatterDielectric(ray, hit, material.data);
    }

    case EMISSIVE: {
        return scatterEmissive(ray, hit, material.data);
    }

    default: {
        ScatterResult res;
        res.scattered = false;
        return res;
    }
    }
}


}  // namespace core