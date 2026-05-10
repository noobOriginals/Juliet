#include <core/scene.hpp>

// Std includes
#include <limits>

namespace core {

void addObjectToScene(const Object& obj, Scene& scene) {
    scene.objects.push_back(obj);
}

void addMaterialToScene(const Material& mat, Scene& scene) {
    scene.materials.push_back(mat);
}

int32 getClosestHit(const Ray& ray, HitRecord& hit, const Scene& scene) {
    int32 matIdx = -1;
    float32 closestT = std::numeric_limits<float32>::max();
    for (const Object& obj : scene.objects) {
        if (hitObject(ray, hit, 1e-4f, closestT, obj)) {
            matIdx = obj.materialIdx;
            closestT = hit.t;
        }
    }
    return matIdx;
}

} // namespace core