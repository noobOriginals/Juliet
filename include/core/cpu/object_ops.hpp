#ifndef CORE_CPU_OBJECT_OPS_HPP
#define CORE_CPU_OBJECT_OPS_HPP

#ifndef OBJECT_OPS
#define OBJECT_OPS
#else
#error OBJECT_OPS already defined! Cannot include use two different backends at the same time!
#endif // OBJECT_OPS

// Lib includes
#include <glm/glm.hpp>

// Local includes
#include "util/types.h"
#include "core/object.hpp"
#include "core/cpu/ray.hpp"
#include "core/cpu/hitrecord.hpp"

namespace core {

void makeSphere(float32 data[OBJECT_DATA_SIZE], const glm::vec3& center, float32 radius);
void makeTriangle(float32 data[OBJECT_DATA_SIZE], const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
void makeQuad(float32 data[OBJECT_DATA_SIZE], const glm::vec3& center, const glm::vec3& u, const glm::vec3& v);
void makeAABB(float32 data[OBJECT_DATA_SIZE], const glm::vec3& bMin, const glm::vec3& bMax);
void makeOBB(float32 data[OBJECT_DATA_SIZE], const glm::vec3& bMin, const glm::vec3& bMax, const glm::vec3& u, const glm::vec3& v);

bool hitSphere(float32 data[OBJECT_DATA_SIZE], const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax);
bool hitTriangle(float32 data[OBJECT_DATA_SIZE], const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax);
bool hitQuad(float32 data[OBJECT_DATA_SIZE], const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax);
bool hitAABB(float32 data[OBJECT_DATA_SIZE], const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax);
bool hitOBB(float32 data[OBJECT_DATA_SIZE], const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax);
bool hitObject(int32 type, float32 data[OBJECT_DATA_SIZE], const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax);

} // namespace core

#endif // CORE_CPU_OBJECT_OPS_HPP
