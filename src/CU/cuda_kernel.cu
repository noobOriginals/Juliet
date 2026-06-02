#include "cu/cuda_kernel.cuh"

#define UTIL_PI 3.141592653589793238462643383279502884197169399375105820974944592307816406286

namespace cu {

struct PCG32 {
    uint64 state, inc;
};

__device__ uint32 pcg32(PCG32& rng) {
    uint64 oldstate = rng.state;
    rng.state = oldstate * 6364136223846793005ull + (rng.inc | 1ull);
    uint32 xorshift = (uint32) (((oldstate >> 18u) ^ oldstate) >> 27u);
    uint32 rotation = (uint32) (oldstate >> 59u);
    return (xorshift >> rotation) | (xorshift << ((32u - rotation) & 31u));
}

__device__ void pcgSeed(PCG32& rng, uint64 seed, uint64 sequence) {
    rng.state = 0ull;
    rng.inc = (sequence << 1u) | 1u;
    pcg32(rng);
    rng.state += seed;
    pcg32(rng);
}

__device__ uint64 hash(uint64 x) {
    x += 0x9E3779B97F4A7C15ull;
    x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9ull;
    x = (x ^ (x >> 27)) * 0x94D049BB133111EBull;
    return x ^ (x >> 31);
}

__device__ float32 nextFloat(PCG32& rng) {
    return (float32) pcg32(rng) * (1.0f / 4294967296.0f);
}

__device__ vec3 randomVec(PCG32& rng) {
    return vec3(nextFloat(rng), nextFloat(rng), nextFloat(rng));
}

__device__ vec3 randomUV(PCG32& rng) {
    float32 u = nextFloat(rng);
    float32 v = nextFloat(rng);
    float32 theta = 2.0f * UTIL_PI * u;
    float32 phi = acos(1.0f - 2.0f * v);
    float32 sinT = sin(theta);
    float32 cosT = cos(theta);
    float32 sinP = sin(phi);
    float32 cosP = cos(phi);
    return vec3(sinP * cosT, sinP * sinT, cosP);
}

__device__ vec3 randomOnHemisphere(PCG32& rng, const vec3& normal) {
    vec3 v = randomUV(rng);
    return dot(v, normal) > 0.0f ? v : -v;
}

__device__ vec3 randomCosineHemisphere(PCG32& rng, const vec3& normal) {
    vec3 v = normalize(normal + randomUV(rng));
    if (dot(v, v) == 0.0f) {
        return normal;
    }
    return v;
}

__device__ vec3 diffuse(PCG32& rng, const vec3& normal) {
    return randomCosineHemisphere(rng, normal);
}

__device__ vec3 reflect(PCG32& rng, const vec3& v, const vec3& normal) {
    return v - 2.0f * dot(v, normal) * normal;
}

__device__ float32 reflectance(float32 cos, float32 n1, float32 n2) {
    float32 r0 = (n1 - n2) / (n1 + n2);
    r0 *= r0;
    return r0 + (1.0f - r0) * pow((1.0f - cos), 5);
}

__device__ vec3 refract(PCG32& rng, const vec3& dir, const vec3& normal, float32 n1, float32 n2) {
    float32 cos = dot(-dir, normal);
    float32 sin = sqrt(max(0.0f, 1.0f - cos * cos));
    float32 idx = n1 / n2;
    if (sin * idx > 1.0f || reflectance(cos, n1, n2) > nextFloat(rng)) {
        return dir + 2.0f * cos * normal;
    }
    vec3 perp = idx * (dir + cos * normal);
    vec3 para = -sqrt(fabs(1.0f - dot(perp, perp))) * normal;
    return perp + para;
}

struct Ray {
    vec3 org, dir;
};

__device__ vec3 rayAt(const Ray& ray, float32 t) {
    return ray.org + ray.dir * t;
}

struct HitRecord {
    float32 t;
    vec3 p, n;
    bool exit;
};

__device__ void setHitNormal(HitRecord& hit, const Ray& ray, const vec3& normal) {
    if (dot(ray.dir, normal) > 0) {
        hit.n = -normal;
        hit.exit = true;
    } else {
        hit.n = normal;
        hit.exit = false;
    }
}

enum ObjectType : int32 {
    SPHERE = 0,
    TRIANGLE = 1,
    QUAD = 2,
    AABB = 3,
    OBB = 4
};

__device__ bool hitSphere(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[OBJECT_DATA_SIZE]) {
    vec3 center(data[0], data[1], data[2]);
    float32 radius = data[3];

    vec3 oc = center - ray.org;

    float32 a = dot(ray.dir, ray.dir);
    float32 h = dot(ray.dir, oc);
    float32 c = dot(oc, oc) - radius * radius;
    float32 det = h * h - a * c;

    if (det < 0.0f) {
        return false;
    }

    float32 sqrtd = sqrt(det);
    float32 invA = 1.0f / a;

    float32 t = (h - sqrtd) * invA;
    if (t < tMin) {
        t = (h + sqrtd) * invA;
    }
    if (t < tMin || t > tMax) {
        return false;
    }

    hit.t = t;
    hit.p = rayAt(ray, t);
    setHitNormal(hit, ray, (hit.p - center) / radius);
    return true;
}

__device__ bool hitTriangle(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[OBJECT_DATA_SIZE]) {
    vec3 a(data[0], data[1], data[2]);
    vec3 b(data[3], data[4], data[5]);
    vec3 c(data[6], data[7], data[8]);
    vec3 ab = b - a;
    vec3 ac = c - a;

    vec3 pvec = cross(ray.dir, ac);
    float32 det = dot(ab, pvec);

    if (fabs(det) <= EPSILON) {
        return false;
    }

    float32 invDet = 1.0f / det;

    vec3 tvec = ray.org - a;
    float32 u = dot(tvec, pvec) * invDet;
    if (u < 0.0f || u > 1.0f) {
        return false;
    }

    vec3 qvec = cross(tvec, ab);
    float32 v = dot(ray.dir, qvec) * invDet;
    if (v < 0.0f || u + v > 1.0f) {
        return false;
    }

    float32 t = dot(ac, qvec) * invDet;
    if (t < tMin || t > tMax) {
        return false;
    }

    hit.t = t;
    hit.p = rayAt(ray, t);
    setHitNormal(hit, ray, normalize(cross(ab, ac)));
    return true;
}

__device__ bool hitQuad(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[OBJECT_DATA_SIZE]) {
    vec3 center(data[0], data[1], data[2]);
    vec3 u(data[3], data[4], data[5]);
    vec3 v(data[6], data[7], data[8]);
    vec3 origin =  center - u * 0.5f - v * 0.5f;
    vec3 normal = normalize(cross(u, v));
    float32 uu = dot(u, u);
    float32 vv = dot(v, v);
    float32 uv = dot(u, v);
    float32 invPD = 1.0f / (uu * vv - uv * uv);

    float32 denom = dot(ray.dir, normal);

    if (fabs(denom) <= EPSILON) {
        return false;
    }

    float32 t = dot(center - ray.org, normal) / denom;
    if (t < tMin || t > tMax) {
        return false;
    }

    vec3 p = rayAt(ray, t);
    vec3 op = p - origin;
    float32 opu = dot(op, u);
    float32 opv = dot(op, v);
    float32 s = (opu * vv - opv * uv) * invPD;
    float32 w = (opv * uu - opu * uv) * invPD;
    if (s < 0.0f || s > 1.0f || w < 0.0f || w > 1.0f) {
        return false;
    }

    hit.t = t;
    hit.p = p;
    setHitNormal(hit, ray, normal);
    return true;
}

__device__ bool hitAABB(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[OBJECT_DATA_SIZE]) {
    vec3 bMin(data[0], data[1], data[2]);
    vec3 bMax(data[3], data[4], data[5]);
    vec3 halfs = (bMax - bMin) * 0.5f;
    vec3 center = bMin + halfs;

    vec3 ro = ray.org - center;
    vec3 rd = ray.dir;

    float32 winding = 1.0f;
    float32 fx = fabs(ro.x) / halfs.x;
    float32 fy = fabs(ro.y) / halfs.y;
    float32 fz = fabs(ro.z) / halfs.z;
    if (fx < 1.0f && fy < 1.0f && fz < 1.0f) {
        winding = -1.0f;
    }

    float32 sgnX = -copysign(1.0f, rd.x);
    float32 sgnY = -copysign(1.0f, rd.y);
    float32 sgnZ = -copysign(1.0f, rd.z);

    float32 dx = (halfs.x * winding * sgnX - ro.x) / rd.x;
    float32 dy = (halfs.y * winding * sgnY - ro.y) / rd.y;
    float32 dz = (halfs.z * winding * sgnZ - ro.z) / rd.z;

    bool hitX = (dx >= 0.0f) & (fabs(ro.y + rd.y * dx) <= halfs.y) & (fabs(ro.z + rd.z * dx) <= halfs.z);
    bool hitY = (dy >= 0.0f) & (fabs(ro.z + rd.z * dy) <= halfs.z) & (fabs(ro.x + rd.x * dy) <= halfs.x);
    bool hitZ = (dz >= 0.0f) & (fabs(ro.x + rd.x * dz) <= halfs.x) & (fabs(ro.y + rd.y * dz) <= halfs.y);

    if (!hitX && !hitY && !hitZ) {
        return false;
    }

    float32 t = hitX ? dx : (hitY ? dy : dz);
    if (t < tMin || t > tMax) {
        return false;
    }

    hit.t = t;
    hit.p = rayAt(ray, t);
    hit.n = hitX ? vec3(sgnX, 0.0f, 0.0f) : (hitY ? vec3(0.0f, sgnY, 0.0f) : vec3(0.0f, 0.0f, sgnZ));
    hit.exit = (winding < 0.0f);
    return true;
}

__device__ bool hitOBB(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[OBJECT_DATA_SIZE]) {
    vec3 bMin(data[0], data[1], data[2]);
    vec3 bMax(data[3], data[4], data[5]);
    vec3 halfs = (bMax - bMin) * 0.5f;
    vec3 center = bMin + halfs;
    vec3 u = vec3(data[6], data[7], data[8]);
    vec3 v = vec3(data[9], data[10], data[11]);
    vec3 w = cross(u, v);

    vec3 ro = ray.org - center;
    vec3 rd = ray.dir;
    ro = vec3(dot(ro, u), dot(ro, v), dot(ro, w));
    rd = vec3(dot(rd, u), dot(rd, v), dot(rd, w));

    float32 winding = 1.0f;
    float32 fx = fabs(ro.x) / halfs.x;
    float32 fy = fabs(ro.y) / halfs.y;
    float32 fz = fabs(ro.z) / halfs.z;
    if (fx < 1.0f && fy < 1.0f && fz < 1.0f) {
        winding = -1.0f;
    }

    float32 sgnX = -copysign(1.0f, rd.x);
    float32 sgnY = -copysign(1.0f, rd.y);
    float32 sgnZ = -copysign(1.0f, rd.z);

    float32 dx = (halfs.x * winding * sgnX - ro.x) / rd.x;
    float32 dy = (halfs.y * winding * sgnY - ro.y) / rd.y;
    float32 dz = (halfs.z * winding * sgnZ - ro.z) / rd.z;

    bool hitX = (dx >= 0.0f) & (fabs(ro.y + rd.y * dx) <= halfs.y) & (fabs(ro.z + rd.z * dx) <= halfs.z);
    bool hitY = (dy >= 0.0f) & (fabs(ro.z + rd.z * dy) <= halfs.z) & (fabs(ro.x + rd.x * dy) <= halfs.x);
    bool hitZ = (dz >= 0.0f) & (fabs(ro.x + rd.x * dz) <= halfs.x) & (fabs(ro.y + rd.y * dz) <= halfs.y);

    if (!hitX && !hitY && !hitZ) {
        return false;
    }

    float32 t = hitX ? dx : (hitY ? dy : dz);
    if (t < tMin || t > tMax) {
        return false;
    }

    vec3 localN = hitX ? vec3(sgnX, 0.0f, 0.0f) : (hitY ? vec3(0.0f, sgnY, 0.0f) : vec3(0.0f, 0.0f, sgnZ));

    hit.t = t;
    hit.p = rayAt(ray, t);
    hit.n = u * localN.x + v * localN.y + w * localN.z;
    hit.exit = (winding < 0.0f);
    return true;
}

__device__ bool hitObject(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const Object& obj) {
    switch (obj.type) {
    case SPHERE: {
        return hitSphere(ray, hit, tMin, tMax, obj.data);
    }

    case TRIANGLE: {
        return hitTriangle(ray, hit, tMin, tMax, obj.data);
    }

    case QUAD: {
        return hitQuad(ray, hit, tMin, tMax, obj.data);
    }

    case AABB: {
        return hitAABB(ray, hit, tMin, tMax, obj.data);
    }

    case OBB: {
        return hitOBB(ray, hit, tMin, tMax, obj.data);
    }

    default: {
        return false;
    }
    }
}

enum MaterialType : int32 {
    DIFFUSE = 0,
    METAL = 1,
    DIELECTRIC = 2,
    EMISSIVE = 3
};

struct ScatterResult {
    Ray ray;
    vec3 albedo;
    bool scattered;
};

__device__ ScatterResult scatterDiffuse(PCG32& rng, const Ray& ray, const HitRecord& hit, const float32 data[MATERIAL_DATA_SIZE]) {
    ScatterResult res;
    res.albedo = vec3(data[0], data[1], data[2]);
    res.scattered = true;
    res.ray.dir = normalize(diffuse(rng, hit.n));
    res.ray.org = hit.p + res.ray.dir * 1e-4f;
    return res;
}

__device__ ScatterResult scatterMetal(PCG32& rng, const Ray& ray, const HitRecord& hit, const float32 data[MATERIAL_DATA_SIZE]) {
    ScatterResult res;
    res.albedo = vec3(data[0], data[1], data[2]);
    res.scattered = true;
    if (nextFloat(rng) < data[4] * abs(dot(ray.dir, hit.n))) {
        res.ray.dir = normalize(reflect(rng, ray.dir, hit.n) + randomUV(rng) * data[3]);
    } else {
        res.ray.dir = normalize(diffuse(rng, hit.n));
    }
    res.ray.org = hit.p + res.ray.dir * 1e-4f;
    return res;
}

__device__ ScatterResult scatterDielectric(PCG32& rng, const Ray& ray, const HitRecord& hit, const float32 data[MATERIAL_DATA_SIZE]) {
    ScatterResult res;
    res.albedo = vec3(data[0], data[1], data[2]);
    res.scattered = true;
    float32 n1 = 1.0f, n2 = data[3];
    if (hit.exit) {
        float32 tmp = n1;
        n1 = n2;
        n2 = tmp;
    }
    res.ray.dir = normalize(refract(rng, ray.dir, hit.n, n1, n2)  + randomUV(rng) * data[4]);
    res.ray.org = hit.p + res.ray.dir * 1e-4f;
    return res;
}

__device__ ScatterResult scatterEmissive(PCG32& rng, const Ray& ray, const HitRecord& hit, const float32 data[MATERIAL_DATA_SIZE]) {
    return scatterDiffuse(rng, ray, hit, data);
}

__device__ ScatterResult scatterMaterial(PCG32& rng, const Ray& ray, const HitRecord& hit, const Material& material) {
    switch (material.type) {
    case DIFFUSE: {
        return scatterDiffuse(rng, ray, hit, material.data);
    }

    case METAL: {
        return scatterMetal(rng, ray, hit, material.data);
    }

    case DIELECTRIC: {
        return scatterDielectric(rng, ray, hit, material.data);
    }

    case EMISSIVE: {
        return scatterEmissive(rng, ray, hit, material.data);
    }

    default: {
        ScatterResult res;
        res.scattered = false;
        return res;
    }
    }
}

// Render

struct ivec2 {
    uint32 x, y;
};

__global__ void kernel(
    const RenderParams* p,
    const Object* objects,
    const Material* materials,
    const SceneData* sceneData,
    vec3* pixels
) {
    ivec2 id;
    id.x = blockDim.x * blockIdx.x + threadIdx.x;
    id.y = blockDim.y * blockIdx.y + threadIdx.y;

    uint32 width = (uint32) p->screenW;
    uint32 height = (uint32) p->screenH;
    if (id.x >= width || id.y >= height) {
        return;
    }

    PCG32 rng;
    vec3 pixel = p->pixelOrigin + p->pixelDeltaW * id.x + p->pixelDeltaH * id.y;

    Ray ray;

    vec3 totalColor(0.0f);

    for (int32 i = 0; i < p->samplesPerPixel; i++) {
        uint64 seed = hash(i);
        uint64 sequence = hash(id.y * width + id.x);
        pcgSeed(rng, seed, sequence);
        vec3 jitter = p->pixelDeltaW * (nextFloat(rng) - 0.5f) + p->pixelDeltaH * (nextFloat(rng) - 0.5f);

        ray.org = p->camPos;
        ray.dir = normalize(pixel + jitter - p->camPos);

        HitRecord hit;
        ScatterResult sres;

        vec3 radiance(0.0f);
        vec3 throughput(1.0f);
        vec3 emission(0.0f);

        for (int32 j = 0; j < p->maxBounces; j++) {
            int32 mIdx = -1;
            float32 closestT = INFINITY;
            for (uint64 o = 0; o < sceneData->oCount; o++) {
                Object obj = objects[o];
                if (hitObject(ray, hit, 1e-4f, closestT, obj)) {
                    closestT = hit.t;
                    mIdx = obj.mIdx;
                }
            }

            if (mIdx < 0) {
                break;
            }

            Material mat = materials[mIdx];
            sres = scatterMaterial(rng, ray, hit, mat);

            if (!sres.scattered) {
                break;
            }

            emission = vec3(0.0f);
            if (mat.type == EMISSIVE) {
                emission = sres.albedo;
            } else {
                throughput *= sres.albedo;
            }
            radiance += emission * throughput;

            ray = sres.ray;
        }

        totalColor += radiance;
    }

    totalColor /= p->samplesPerPixel;
    pixels[id.y * width + id.x] = totalColor;
}

} // namespace cu
