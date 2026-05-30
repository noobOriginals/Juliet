#ifndef CORE_HITRECORD_HPP
#define CORE_HITRECORD_HPP

// Lib includes
#include <glm/glm.hpp>

// Local includes
#include "util/types.h"
#include "core/ray.hpp"

namespace core {

struct HitRecord {
    float32 t;
    glm::vec3 p, n;
    bool exit;
};

inline void setHitNormal(HitRecord& hr, const Ray& r, const glm::vec3& normal) {
    if (glm::dot(r.dir, normal) > 0.0f) {
        hr.n = -normal;
        hr.exit = true;
    } else {
        hr.n = normal;
        hr.exit = false;
    }
}

}  // namespace core

#endif  // CORE_HITRECORD_HPP