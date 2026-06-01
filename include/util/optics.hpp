#ifndef OPTICS_HPP
#define OPTICS_HPP

// Std includes
#include <cmath>

// Lib includes
#include <glm/glm.hpp>

// Local includes
#include "util/random.hpp"

namespace util {

inline glm::vec3 diffuse(PCG32& rng, const glm::vec3& normal) {
    return randomCosineHemisphere(rng, normal);
}

inline glm::vec3 reflect(const glm::vec3& v, const glm::vec3& normal) {
    return v - 2.0f * glm::dot(v, normal) * normal;
}

inline float32 reflectance(float32 cos, float32 n1, float32 n2) {
    float32 r0 = (n1 - n2) / (n1 + n2);
    r0 *= r0;
    return r0 + (1.0f - r0) * std::pow(1.0f - cos, 5);
}

inline glm::vec3 refract(PCG32& rng, const glm::vec3& dir, const glm::vec3& normal, float32 n1, float32 n2) {
    float32 cos = std::max(0.0f, -glm::dot(dir, normal));
    float32 idx = n1 / n2;
    float32 r = 0.0f;
    if ((1.0f - cos * cos) * idx * idx > 1.0f) {
        r = 1.0f;
    } else {
        r = reflectance(cos, n1, n2);
    }
    if (r > nextFloat(rng)) {
        return dir + 2.0f * cos * normal;
    }
    glm::vec3 perp = idx * (dir + cos * normal);
    glm::vec3 para = -std::sqrt(std::fabs(1.0f - glm::dot(perp, perp))) * normal;
    return perp + para;
}

} // namespace util

#endif
