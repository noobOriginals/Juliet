// Std includes
#include <iostream>

#include "core/cpu/material_ops.hpp"
#include "util/random.hpp"

int main() {
    float32 data[core::MATERIAL_DATA_SIZE];
    core::makeMaterial(data, glm::vec3(1.0f), glm::vec3(0.0f), 0.0f, 1.5f);
    return 0;
}
