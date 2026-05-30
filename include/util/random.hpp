#ifndef UTIL_RANDOM_HPP
#define UTIL_RANDOM_HPP

// Std includes
#include <cmath>

// Lib includes
#include <glm/glm.hpp>

// Local includes
#include "util/types.h"
#include "util/util.hpp"

namespace util {

inline uint64 hash(uint64 x) {
    x += 0x9E3779B97F4A7C15ull;
    x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9ull;
    x = (x ^ (x >> 27)) * 0x94D049BB133111EBull;
    return x ^ (x >> 31);
}

struct PCG32 {
    uint64 state, inc;
};

inline uint32 pcg32(PCG32& rng) {
    uint64 oldstate = rng.state;
    rng.state = oldstate * 6364136223846793005ull + (rng.inc | 1ull);
    uint32 xorshift = (uint32) (((oldstate >> 18u) ^ oldstate) >> 27u);
    uint32 rotation = (uint32) (oldstate >> 59u);
    return (xorshift >> rotation) | (xorshift << ((32u - rotation) & 31u));
}

inline void pcgSeed(PCG32& rng, uint64 seed, uint64 sequence) {
    rng.state = 0ull;
    rng.inc = (sequence << 1u) | 1u;
    pcg32(rng);
    rng.state += seed;
    pcg32(rng);
}

inline float32 nextFloat(PCG32& rng) {
    return (float32) pcg32(rng) * (1.0f / 4294967296.0f);
}

inline glm::vec3 randomVec(PCG32& rng) {
    return glm::vec3(nextFloat(rng), nextFloat(rng), nextFloat(rng));
}

inline glm::vec3 randomUV(PCG32& rng) {
    float32 u = nextFloat(rng);
    float32 v = nextFloat(rng);
    float32 theta = 2.0f * UTIL_PI * u;
    float32 phi = std::acos(1.0f - 2.0f * v);
    float32 sinP = std::sin(phi);
    return glm::vec3(sinP * std::cos(theta), sinP * std::sin(theta), std::cos(phi));
}

inline glm::vec3 randomOnHemisphere(PCG32& rng, const glm::vec3& normal) {
    glm::vec3 v = randomUV(rng);
    return glm::dot(v, normal) > 0.0f ? v : -v;
}

inline glm::vec3 randomCosineHemisphere(PCG32& rng, const glm::vec3& normal) {
    glm::vec3 v = glm::normalize(normal + randomUV(rng));
    return v;
}

}

#endif // UTIL_RANDOM_HPP
