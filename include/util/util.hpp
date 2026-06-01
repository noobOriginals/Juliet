#ifndef UTIL_UTIL_HPP
#define UTIL_UTIL_HPP

// Std includes
#include <cmath>

// Lib includes
#include <glm/glm.hpp>

// Local includes
#include "util/types.h"

constexpr float64 UTIL_PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286;
constexpr float64 UTIL_INV_PI = 1.0 / UTIL_PI;

namespace util {

inline double degToRad(double deg) {
    return deg * UTIL_PI / 180.0;
}

inline double radToDeg(double rad) {
    return rad * 180.0 / UTIL_PI;
}

inline float gammaCorrect(float val) {
    if (val <= 0.0f) {
        return 0.0f;
    }
    if (val <= 0.0031308f) {
        return 12.92f * val;
    }
    return 1.055f * std::pow(val, 1.0f / 2.4f) - 0.055f;
}

inline glm::vec3 gammaCorrect(const glm::vec3& color) {
    return glm::vec3(gammaCorrect(color.x), gammaCorrect(color.y), gammaCorrect(color.z));
}

inline int32 clamp(int32 val, int32 min, int32 max) {
    if (val < min) {
        return min;
    }
    if (val > max) {
        return max;
    }
    return val;
}

inline float clamp(float val, float min, float max) {
    if (val < min) {
        return min;
    }
    if (val > max) {
        return max;
    }
    return val;
}

inline glm::vec3 clamp(const glm::vec3& vec, float min, float max) {
    return glm::vec3(clamp(vec.x, min, max), clamp(vec.y, min, max), clamp(vec.z, min, max));
}

inline glm::vec3 exp(const glm::vec3& vec) {
    return glm::vec3(std::exp(vec.x), std::exp(vec.y), std::exp(vec.z));
}

} // namespace util

#endif  // UTIL_UTIL_HPP