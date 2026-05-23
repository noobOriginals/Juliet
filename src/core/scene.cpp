#include <core/scene.hpp>

// Lib includes
#include <glm/glm.hpp>

// Std includes
#include <fstream>
#include <string>
#include <iostream>
#include <limits>

#define tryreadReturnVoid(x) if (!(x)) { std::cerr << "Read failed\n"; return; }
#define tryreadReturnX(x, ret) if (!(x)) { std::cerr << "Read failed\n"; return ret; }

namespace core {

const char constexpr* OBJECT_NAMES[] = {"SPHERE", "TRIANGLE", "QUAD", "AABB", "OBB"};
const char constexpr* MATERIAL_NAMES[] = {"DIFFUSE", "METAL", "DIELECTRIC", "EMMISIVE"};

std::string toString(const Object& obj) {
    std::string str;
    str += std::string(OBJECT_NAMES[obj.type]) + " ";
    for (int32 i = 0; i < OBJECT_DATA_SIZE; i++) {
        str += std::to_string(obj.data[i]) + " ";
    }
    str += std::to_string(obj.materialIdx);
    return str;
}

std::string toString(const Material& mat) {
    std::string str;
    str += std::string(MATERIAL_NAMES[mat.type]) + " ";
    for (int32 i = 0; i < MATERIAL_DATA_SIZE; i++) {
        str += std::to_string(mat.data[i]) + " ";
    }
    return str;
}

Scene loadSceneFromFile(const char* filepath) {
    Scene scene = {};
    std::cout << "Loading scene from \"" << filepath << "\"...\n";
    std::fstream file(filepath, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Failed to open \"" << filepath << "\"!\n";
        return scene;
    }
    std::string type;
    while ((file >> type)) {
        for (int32 i = 0; i < sizeof(OBJECT_NAMES) / sizeof(const char*); i++) {
            if (type == OBJECT_NAMES[i]) {
                Object obj;
                obj.type = i;
                for (int32 j = 0; j < OBJECT_DATA_SIZE; j++) {
                    tryreadReturnX(file >> obj.data[j], scene)
                }
                tryreadReturnX(file >> i, scene)
                obj.materialIdx = i;
                scene.objects.push_back(obj);
                break;
            }
        }
        for (int32 i = 0; i < sizeof(MATERIAL_NAMES) / sizeof(const char*); i++) {
            if (type == MATERIAL_NAMES[i]) {
                Material mat;
                mat.type = i;
                for (int32 j = 0; j < MATERIAL_DATA_SIZE; j++) {
                    tryreadReturnX(file >> mat.data[j], scene)
                }
                scene.materials.push_back(mat);
                break;
            }
        }
    }
    file.close();
    scene.isValid = true;
    std::cout << "Loaded!\n";
    return scene;
}

void saveSceneToFile(const char* filepath, const Scene& scene) {
    std::fstream file(filepath, std::ios::out);
    for (const Object o : scene.objects) {
        file << toString(o) << "\n";
    }
    file << "\n";
    for (const Material m : scene.materials) {
        file << toString(m) << "\n";
    }
    file.close();
}

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