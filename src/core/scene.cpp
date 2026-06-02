#include "core/scene.hpp"

namespace core {

void addObjectToSceneData(SceneData& data, float32 objData[OBJECT_DATA_SIZE], int32 objType, int32 matIdx) {
    for (int32 i = 0; i < OBJECT_DATA_SIZE; i++) {
        data.objectData.push_back(objData[i]);
    }
    data.objectTypes.push_back(objType);
    data.materialIndices.push_back(matIdx);
}

void addMaterialToSceneData(SceneData& data, float32 matData[MATERIAL_DATA_SIZE]) {
    for (int32 i = 0; i < MATERIAL_DATA_SIZE; i++) {
        data.materialData.push_back(matData[i]);
    }
}

Scene::Scene(const std::string& filepath) {

}

Scene::Scene(SceneParameters params, SceneData data) {

}

void Scene::addObject(float32 objData[OBJECT_DATA_SIZE], int32 objType, int32 matIdx) {
    addObjectToSceneData(data, objData, objType, matIdx);
}

void Scene::addMaterial(float32 matData[MATERIAL_DATA_SIZE]) {
    addMaterialToSceneData(data, matData);
}

void Scene::save(const std::string& filepath) {

}

void Scene::render() {

}

} // namespace core
