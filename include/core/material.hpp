#ifndef CORE_MATERIAL_HPP
#define CORE_MATERIAL_HPP

// Local includes
#include "util/types.h"

namespace core {

const uint64 MAT_ALBEDO_R = 0;
const uint64 MAT_ALBEDO_G = 1;
const uint64 MAT_ALBEDO_B = 2;
const uint64 MAT_EMISSION_R = 3;
const uint64 MAT_EMISSION_G = 4;
const uint64 MAT_EMISSION_B = 5;
const uint64 MAT_FUZZ = 6;
const uint64 MAT_REF_IDX = 7;

const uint64 MATERIAL_DATA_SIZE = 8;

enum MaterialType : int32 {
    INVALID = 0,
    DIFFUSE = 1,
    METAL = 2,
    DIELECTRIC = 3
};

} // namespace core

#endif // CORE_MATERIAL_HPP
