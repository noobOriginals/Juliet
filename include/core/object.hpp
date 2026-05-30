#ifndef CORE_OBJECT_HPP
#define CORE_OBJECT_HPP

// Local includes
#include "util/types.h"

namespace core {

const uint64 OBJECT_DATA_SIZE = 12;

enum ObjectType : int32 {
    INVALID = 0,
    SPHERE = 1,
    TRIANGLE = 2,
    QUAD = 3,
    AABB = 4,
    OBB = 5
};

} // namespace core

#endif // CORE_OBJECT_HPP
