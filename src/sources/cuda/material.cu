#include <cuda/material.cuh>

// Local includes
#include <cuda/optics.cuh>

namespace cuda {

__device__ ScatterResult scatterDiffuse(curandStatePhilox4_32_10* randState, const Ray& ray, const HitRecord& hit, const float32 data[core::MATERIAL_DATA_SIZE]) {
    ScatterResult res;
    res.albedo = vec3(data[0], data[1], data[2]);
    res.scattered = true;
    res.ray.org = hit.p;
    res.ray.dir = diffuse(randState, hit.n);
    return res;
}

__device__ ScatterResult scatterMetal(curandStatePhilox4_32_10* randState, const Ray& ray, const HitRecord& hit, const float32 data[core::MATERIAL_DATA_SIZE]) {
    ScatterResult res;
    res.albedo = vec3(data[0], data[1], data[2]);
    res.scattered = true;
    res.ray.org = hit.p;
    res.ray.dir = reflect(randState, ray.dir, hit.n, data[3]);
    return res;
}

__device__ ScatterResult scatterDielectric(curandStatePhilox4_32_10* randState, const Ray& ray, const HitRecord& hit, const float32 data[core::MATERIAL_DATA_SIZE]) {
    ScatterResult res;
    res.albedo = vec3(data[0], data[1], data[2]);
    res.scattered = true;
    res.ray.org = hit.p;
    float32 n1 = 1.0f, n2 = data[3];
    if (hit.exit) {
        float32 tmp = n1;
        n1 = n2;
        n2 = tmp;
    }
    res.ray.dir = refract(randState, ray.dir, hit.n, n1, n2);
    return res;
}

__device__ ScatterResult scatterEmmisive(curandStatePhilox4_32_10* randState, const Ray& ray, const HitRecord& hit, const float32 data[core::MATERIAL_DATA_SIZE]) {
    return scatterDiffuse(randState, ray, hit, data);
}

__device__ ScatterResult scatterMaterial(curandStatePhilox4_32_10* randState, const Ray& ray, const HitRecord& hit, const Material& material) {
    switch (material.type) {
    case core::DIFFUSE: {
        return scatterDiffuse(randState, ray, hit, material.data);
    }

    case core::METAL: {
        return scatterMetal(randState, ray, hit, material.data);
    }

    case core::DIELECTRIC: {
        return scatterDielectric(randState, ray, hit, material.data);
    }

    case core::EMMISIVE: {
        return scatterEmmisive(randState, ray, hit, material.data);
    }

    default: {
        ScatterResult res;
        res.scattered = false;
        return res;
    }
    }
}

} // namespace cuda
