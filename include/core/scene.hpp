#ifndef CORE_SCENE_HPP
#define CORE_SCENE_HPP

// Std includes
#include <string>
#include <vector>

// Lib includes
#include <glm/glm.hpp>

// Local includes
#include "util/types.h"
#include "core/object.hpp"
#include "core/material.hpp"

namespace core {

struct SceneParameters {
    int32 screenW, screenH;
    float32 hfov, focalLength;
    glm::vec3 camPos, camLookAt, worldUp;
    int32 maxBounces, samplesPerPixel;
};

struct SceneData {
    std::vector<float32> objectData;
    std::vector<int32> objectTypes;
    std::vector<int32> materialIndices;
    std::vector<float32> materialData;
};

void addObjectToSceneData(SceneData& data, float32 objData[OBJECT_DATA_SIZE], int32 objType, int32 matIdx);
void addMaterialToSceneData(SceneData& data, float32 matData[MATERIAL_DATA_SIZE]);

class Scene {
public:
    Scene(const std::string& filepath);
    Scene(SceneParameters params, SceneData data);

    void addObject(float32 objData[OBJECT_DATA_SIZE], int32 objType, int32 matIdx);
    void addMaterial(float32 matData[MATERIAL_DATA_SIZE]);

    void save(const std::string& filepath);

    void render();

private:
    int32 screenW, screenH;
    glm::vec3 camPos, pixelOrigin, pixelDeltaW, pixelDeltaH;
    int32 maxBounces, spp;
    SceneData data;
};

} // namespace core

#endif // SCENE+HPP
