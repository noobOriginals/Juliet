#ifndef CORE_MATERIAL_HPP
#define CORE_MATERIAL_HPP

// Local includes
#include <util/types.h>

namespace core {

constexpr uint64 MATERIAL_DATA_SIZE = 4;

enum class MaterialType : int32 {
    DIFFUSE = 0,
    METAL = 1,
    DIELECTRIC = 2,
    EMMISIVE = 3 // TODO
};

struct Material {
    int32 type;
    float32 data[MATERIAL_DATA_SIZE];
};

} // namespace core

#endif  // CORE_MATERIAL_HPP