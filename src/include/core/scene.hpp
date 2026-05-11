#ifndef CORE_SCENE_HPP
#define CORE_SCENE_HPP

// Std includes
#include <vector>

// Local includes
#include <util/types.h>
#include <core/ray.hpp>
#include <core/hitrecord.hpp>
#include <core/object.hpp>
#include <core/material.hpp>

namespace core {

struct Scene {
    std::vector<Object> objects;
    std::vector<Material> materials;
};

Scene loadSceneFromFile(const char* filepath);
void saveSceneToFile(const char* filepath, const Scene& scene);
void addObjectToScene(const Object& obj, Scene& scene);
void addMaterialToScene(const Material& mat, Scene& scene);
int32 getClosestHit(const Ray& ray, HitRecord& hit, const Scene& scene);

} // namespace core

#endif // CORE_SCENE_HPP