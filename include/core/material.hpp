#ifndef CORE_MATERIAL_HPP
#define CORE_MATERIAL_HPP

// Local includes
#include "util/types.h"

namespace core {

const uint64 MAT_COLOR_R = 0;
const uint64 MAT_COLOR_G = 1;
const uint64 MAT_COLOR_B = 2;
const uint64 MAT_EMISSION_R = 3;
const uint64 MAT_EMISSION_G = 4;
const uint64 MAT_EMISSION_B = 5;
const uint64 MAT_ROUGHNESS = 6;
const uint64 MAT_METALLIC = 7;
const uint64 MAT_TRANSLUCENT = 8;
const uint64 MAT_IOR = 9;
const uint64 MAT_SPECULAR = 10;
const uint64 MAT_ABSORBTION = 11;

const uint64 MATERIAL_DATA_SIZE = 12;

} // namespace core

#endif // CORE_MATERIAL_HPP
