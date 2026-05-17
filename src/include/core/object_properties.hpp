#ifndef CORE_OBJECT_PROPERTIES_HPP
#define CORE_OBJECT_PROPERTIES_HPP

// Local includes
#include <util/types.h>

namespace core {

constexpr uint64 OBJECT_DATA_SIZE = 12;

enum ObjectType : int32 {
    SPHERE = 0,
    TRIANGLE = 1,
    QUAD = 2,
    AABB = 3,
    OBB = 4
};

} // namespace core

#endif // CORE_OBJECT_PROPERTIES_HPP