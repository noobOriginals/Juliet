#ifndef CORE_MATERIAL_PROPERTIES_HPP
#define CORE_MATERIAL_PROPERTIES_HPP

// Local includes
#include <util/types.h>

namespace core {

constexpr uint64 MATERIAL_DATA_SIZE = 4;

enum MaterialType : int32 {
    DIFFUSE = 0,
    METAL = 1,
    DIELECTRIC = 2,
    EMMISIVE = 3
};

} // namespace core

#endif // CORE_MATERIAL_PROPERTIES_HPP