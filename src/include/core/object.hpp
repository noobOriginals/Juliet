#ifndef CORE_OBJECT_HPP
#define CORE_OBJECT_HPP

// Lib includes
#include <glm/glm.hpp>

// Local includes
#include <util/types.h>
#include <core/ray.hpp>
#include <core/hitrecord.hpp>

namespace core {

constexpr uint64 OBJECT_DATA_SIZE = 9;

enum ObjectType : int32 {
    SPHERE = 0,
    TRIANGLE = 1,
    QUAD = 2,
    AABB = 3, // TODO
    OBB = 4  // TODO
};

struct Object {
    int32 type;
    float32 data[OBJECT_DATA_SIZE];
    int32 materialIdx;
};

Object makeSphere(const glm::vec3& center, float32 radius);
Object makeTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
Object makeQuad(const glm::vec3& center, const glm::vec3& u, const glm::vec3& v);
Object makeAABB(const glm::vec3& bMin, const glm::vec3& bMax);
Object makeOBB(const glm::vec3& center, const glm::vec3& u, const glm::vec3& v);

bool hitSphere(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[OBJECT_DATA_SIZE]);
bool hitTriangle(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[OBJECT_DATA_SIZE]);
bool hitQuad(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[OBJECT_DATA_SIZE]);
bool hitAABB(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[OBJECT_DATA_SIZE]);
bool hitOBB(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[OBJECT_DATA_SIZE]);

} // namespace core

#endif  // CORE_OBJECT_HPP