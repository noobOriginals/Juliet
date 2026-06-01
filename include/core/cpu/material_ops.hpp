#ifndef CORE_CPU_MATERIAL_OPS_HPP
#define CORE_CPU_MATERIAL_OPS_HPP

#ifndef MATERIAL_OPS
#define MATERIAL_OPS
#else
#error MATERIAL_OPS already defined! Cannot use two different backends at the same time!
#endif // MATERIAL_OPS

// Lib includes
#include <glm/glm.hpp>

// Local includes
#include "util/types.h"
#include "util/random.hpp"
#include "core/material.hpp"
#include "core/cpu/hitrecord.hpp"

namespace core {

void makeMaterial(float32 data[MATERIAL_DATA_SIZE], const glm::vec3& albedo, const glm::vec3& emission, float32 roughness, float32 metallic, float32 translucent, float32 ior);

struct BSDFSample {
    glm::vec3 wi;
    bool translucent = false;
};

BSDFSample sampleMaterial(util::PCG32& rng, float32 data[MATERIAL_DATA_SIZE], const HitRecord& hit, const glm::vec3& wo);
glm::vec3 evalMaterial(float32 data[MATERIAL_DATA_SIZE], const BSDFSample& s, const HitRecord& hit, const glm::vec3& wo, const glm::vec3& wi);
float32 pdfMaterial(const BSDFSample& s, const HitRecord& hit, const glm::vec3& wi);

} // namespace core

#endif // CORE_CPU_MATERIAL_OPS_HPP
