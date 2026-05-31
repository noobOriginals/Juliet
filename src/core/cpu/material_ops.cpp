#include "core/cpu/material_ops.hpp"

// Local includes
#include "util/optics.hpp"

namespace core {

void makeMaterial(float32 data[MATERIAL_DATA_SIZE], const glm::vec3& albedo, const glm::vec3& emission, float32 fuzz, float32 refIdx) {
    data[MAT_ALBEDO_R] = albedo.x;
    data[MAT_ALBEDO_G] = albedo.y;
    data[MAT_ALBEDO_B] = albedo.z;
    data[MAT_EMISSION_R] = emission.x;
    data[MAT_EMISSION_G] = emission.y;
    data[MAT_EMISSION_B] = emission.z;
    data[MAT_FUZZ] = fuzz;
    data[MAT_REF_IDX] = refIdx;
}

} // namespace core
