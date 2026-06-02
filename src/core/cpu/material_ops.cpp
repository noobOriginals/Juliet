#include "core/cpu/material_ops.hpp"

// Local includes
#include "core/material.hpp"
#include "glm/geometric.hpp"
#include "util/random.hpp"
#include "util/util.hpp"
#include "util/optics.hpp"

namespace core {

void makeMaterial(float32 data[MATERIAL_DATA_SIZE], const glm::vec3& albedo, const glm::vec3& emission, float32 roughness, float32 metallic, float32 translucent, float32 ior) {
    data[MAT_COLOR_R] = albedo.x;
    data[MAT_COLOR_G] = albedo.y;
    data[MAT_COLOR_B] = albedo.z;
    data[MAT_EMISSION_R] = emission.x;
    data[MAT_EMISSION_G] = emission.y;
    data[MAT_EMISSION_B] = emission.z;
    data[MAT_ROUGHNESS] = roughness;
    data[MAT_METALLIC] = metallic;
    data[MAT_TRANSLUCENT] = translucent;
    data[MAT_IOR] = ior;
}

BSDFSample sampleMaterial(util::PCG32& rng, float32 data[MATERIAL_DATA_SIZE], const HitRecord& hit, const glm::vec3& wo) {
    BSDFSample s{};
    if (data[MAT_TRANSLUCENT] > util::nextFloat(rng)) {
        s.translucent = true;
        float32 n1 = 1.0f;
        float32 n2 = data[MAT_IOR];
        if (hit.exit) {
            float32 temp = n1;
            n1 = n2;
            n2 = temp;
        }
        s.wi = glm::normalize(util::refract(rng, wo, hit.n, n1, n2) + util::randomUV(rng) * data[MAT_ROUGHNESS]);
        return s;
    }
    if (data[MAT_METALLIC] * glm::dot(wo, hit.n) < util::nextFloat(rng)) {
        s.wi = glm::normalize(util::reflect(wo, hit.n) + util::randomUV(rng));
    } else {
        s.wi = util::diffuse(rng, hit.n);
    }
    return s;
}

glm::vec3 evalMaterial(float32 data[MATERIAL_DATA_SIZE], const BSDFSample& s, const HitRecord& hit, const glm::vec3& wo, const glm::vec3& wi) {
    glm::vec3 color(data[MAT_COLOR_R], data[MAT_COLOR_G], data[MAT_COLOR_B]);
    if (s.translucent) {
        if (hit.exit) {
            return util::exp(-hit.t * (1.0f - color) * data[MAT_ABSORBTION]);
        } else {
            return glm::vec3(1.0f);
        }
    }
    return color;
}

float32 pdfMaterial(const BSDFSample& s, const HitRecord& hit, const glm::vec3& wi) {
    return 1.0f;
}

} // namespace core
