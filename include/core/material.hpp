#ifndef CORE_MATERIAL_HPP
#define CORE_MATERIAL_HPP

// Local includes
#include "util/types.h"

namespace core {

const uint64 MATERIAL_DATA_SIZE = 7;

enum MaterialType : int32 {
    INVALID = 0,
    DIFFUSE = 1,
    METAL = 2,
    DIELECTRIC = 3
};

} // namespace core

#endif // CORE_MATERIAL_HPP
