#ifndef CORE_MATERIAL_HPP
#define CORE_MATERIAL_HPP

// Lib includes
#include <glm/glm.hpp>

// Local includes
#include <util/types.h>
#include <core/ray.hpp>
#include <core/hitrecord.hpp>

namespace core {

constexpr uint64 MATERIAL_DATA_SIZE = 4;

enum MaterialType : int32 {
    DIFFUSE = 0,
    METAL = 1,
    DIELECTRIC = 2,
    EMISSIVE = 3
};

struct Material {
    int32 type;
    float32 data[MATERIAL_DATA_SIZE];
};

Material makeDiffuse(const glm::vec3& albedo);
Material makeMetal(const glm::vec3& albedo, float32 fuzz);
Material makeDielectric(const glm::vec3& albedo, float32 refIdx);
Material makeEmissive(const glm::vec3& albedo);

struct ScatterResult {
    Ray ray;
    glm::vec3 albedo;
    bool scattered;
};

ScatterResult scatterDiffuse(const Ray& ray, const HitRecord& hit, const float32 data[MATERIAL_DATA_SIZE]);
ScatterResult scatterMetal(const Ray& ray, const HitRecord& hit, const float32 data[MATERIAL_DATA_SIZE]);
ScatterResult scatterDielectric(const Ray& ray, const HitRecord& hit, const float32 data[MATERIAL_DATA_SIZE]);
ScatterResult scatterEmissive(const Ray& ray, const HitRecord& hit, const float32 data[MATERIAL_DATA_SIZE]);
ScatterResult scatterMaterial(const Ray& ray, const HitRecord& hit, const Material& material);

} // namespace core

#endif  // CORE_MATERIAL_HPP