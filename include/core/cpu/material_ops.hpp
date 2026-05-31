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
#include "core/cpu/ray.hpp"
#include "core/cpu/hitrecord.hpp"

namespace core {

void makeMaterial(float32 data[MATERIAL_DATA_SIZE], const glm::vec3& albedo, const glm::vec3& emission, float32 fuzz, float32 refIdx);

struct BSDFSample {
    glm::vec3 wi;
    glm::vec3 weight;
    float32 pdf;
    bool delta = false;
    bool valid = false;
};

glm::vec3 evalMaterial(int32 type, const float32 data[MATERIAL_DATA_SIZE], const glm::vec3& wi, const glm::vec3& wo, const HitRecord& hit);
float32 pdfMaterial(int32 type, const glm::vec3& wi, const glm::vec3& wo, const HitRecord& hit);
BSDFSample sampleMaterial(int32 type, util::PCG32& rng, const glm::vec3& wo, const HitRecord& hit);

} // namespace core

#endif // CORE_CPU_MATERIAL_OPS_HPP
