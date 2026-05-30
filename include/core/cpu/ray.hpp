#ifndef CORE_RAY_HPP
#define CORE_RAY_HPP

#ifndef RAY
#define RAY
#else
#error RAY already defined! Cannot include use two different backends at the same time!
#endif // RAY

// Lib includes
#include <glm/glm.hpp>

// Local includes
#include "util/types.h"

namespace core {

struct Ray {
    glm::vec3 org, dir;
};

inline glm::vec3 rayAt(const Ray& r, float32 t) {
    return r.org + r.dir * t;
}

} // namespace core

#endif // CORE_RAY_HPP