#ifndef CORE_CPU_MATERIAL_OPS_HPP
#define CORE_CPU_MATERIAL_OPS_HPP

#ifndef MATERIAL_OPS
#define MATERIAL_OPS
#else
#error MATERIAL_OPS already defined! Cannot include use two different backends at the same time!
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

void makeDiffuse(float32 data[MATERIAL_DATA_SIZE], const glm::vec3 albedo);
void makeMetal(float32 data[MATERIAL_DATA_SIZE], const glm::vec3 albedo, float32 fuzz);
void makeDielectric(float32 data[MATERIAL_DATA_SIZE], const glm::vec3 albedo, float32 refIdx);
void makeEmissiveDiffuse(float32 data[MATERIAL_DATA_SIZE], const glm::vec3 albedo, const glm::vec3 emission);
void makeEmissiveMetal(float32 data[MATERIAL_DATA_SIZE], const glm::vec3 albedo, float32 fuzz, const glm::vec3 emission);
void makeEmissiveDielectric(float32 data[MATERIAL_DATA_SIZE], const glm::vec3 albedo, float32 refIdx, const glm::vec3 emission);

struct ScatterResult {
    Ray ray;
    glm::vec3 albedo, emission;
    bool valid = false;
};

ScatterResult scatterDiffuse(float32 data[MATERIAL_DATA_SIZE], util::PCG32& rng, const Ray& ray, const HitRecord& hit);
ScatterResult scatterMetal(float32 data[MATERIAL_DATA_SIZE], util::PCG32& rng, const Ray& ray, const HitRecord& hit);
ScatterResult scatterDielectric(float32 data[MATERIAL_DATA_SIZE], util::PCG32& rng, const Ray& ray, const HitRecord& hit);
ScatterResult scatterMaterial(int32 type, float32 data[MATERIAL_DATA_SIZE], util::PCG32& rng, const Ray& ray, const HitRecord& hit);

} // namespace core

#endif // CORE_CPU_MATERIAL_OPS_HPP
