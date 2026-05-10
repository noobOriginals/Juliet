#ifndef UTIL_UTIL_HPP
#define UTIL_UTIL_HPP

// Std includes
#include <random>

// Lib includes
#include <glm/glm.hpp>

// Local includes
#include <util/types.h>

#define UTIL_PI 3.141592653589793238462643383279502884197169399375105820974944592307816406286

namespace util {

inline float64 degToRad(float64 deg) {
    return deg * UTIL_PI / 180.0;
}

inline float64 radToDeg(float64 rad) {
    return rad * 180.0 / UTIL_PI;
}

inline float32 gammaCorrect(float32 val) {
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

inline float32 clamp(float32 val, float32 min, float32 max) {
    if (val < min) {
        return min;
    }
    if (val > max) {
        return max;
    }
    return val;
}

inline glm::vec3 clamp(const glm::vec3& vec, float32 min, float32 max) {
    return glm::vec3(clamp(vec.x, min, max), clamp(vec.y, min, max), clamp(vec.z, min, max));
}
// Random utility

static thread_local std::mt19937 rng(std::random_device{}());
static thread_local std::uniform_real_distribution<float32> dist(0.0f, 1.0f);

inline float32 randomUnit() {
    return dist(rng);
}

inline int32 pickOneOfN(int32 n) {
    return (int32) (randomUnit() * n);
}

inline int32 pickWeightedOneOfN(const std::vector<float32>& weights) {
    std::vector<float32> cummulative;
    int32 sum = 0;
    for (const float32& f : weights) {
        sum += f;
        cummulative.push_back(sum);
    }
    float32 num = randomUnit() * sum;
    int32 idx = 0;
    while (num > cummulative[idx]) {
        idx++;
    }
    return idx;
}

inline float32 randomUnit(float32 min, float32 max) {
    return min + randomUnit() * (max - min);
}

inline glm::vec3 randomVec(const glm::vec3& min, const glm::vec3& max) {
    return glm::vec3(
        randomUnit(min.x, max.x),
        randomUnit(min.y, max.y),
        randomUnit(min.z, max.z)
    );
}

inline glm::vec3 randomUV() {
    float32 u = randomUnit();
    float32 v = randomUnit();

    float32 theta = 2.0f * UTIL_PI * u;
    float32 phi = std::acos(1.0f - 2.0f * v);

    return glm::vec3(
        std::sin(phi) * std::cos(theta),
        std::sin(phi) * std::sin(theta),
        std::cos(phi)
    );
}

inline glm::vec3 randomOnHemisphere(const glm::vec3& normal) {
    glm::vec3 v = randomUV();
    return glm::dot(v, normal) > 0.0f ? v : -v;
}

inline glm::vec3 randomCosineHemisphere(const glm::vec3& normal) {
    glm::vec3 v = glm::normalize(normal + randomUV());
    if (glm::dot(v, v) == 0.0f) {
        return normal;
    }
    return v;
}

// Optics

inline glm::vec3 diffuse(const glm::vec3& normal) {
    return randomCosineHemisphere(normal);
}

inline glm::vec3 reflect(const glm::vec3& v, const glm::vec3& normal) {
    return v - 2.0f * dot(v, normal) * normal;
}

inline float32 reflectance(float32 cos, float32 n1, float32 n2) {
    float32 r0 = (n1 - n2) / (n1 + n2);
    r0 *= r0;
    return r0 + (1.0f - r0) * std::pow((1.0f - cos), 5);
}

inline glm::vec3 refract(const glm::vec3& dir, const glm::vec3& normal, float32 n1, float32 n2) {
    float32 cos = dot(-dir, normal);
    float32 sin = std::sqrt(std::max(0.0f, 1.0f - cos * cos));
    float32 idx = n1 / n2;
    if (sin * idx > 1.0f || reflectance(cos, n1, n2) > randomUnit()) {
        return dir + 2.0f * cos * normal;
    }
    glm::vec3 perp = idx * (dir + cos * normal);
    glm::vec3 para = -std::sqrt(std::fabs(1.0f - glm::dot(perp, perp))) * normal;
    return perp + para;
}

} // namespace util

#endif  // UTIL_UTIL_HPP