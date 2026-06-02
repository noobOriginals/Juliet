//
//  MetalLibTest.metal
//  MetalLibTest
//
//  Created by noobOriginals on 23/05/2026.
//

#include <metal_stdlib>
using namespace metal;

#define EPSILON 1e-8f
#define OBJECT_DATA_SIZE 12
#define MATERIAL_DATA_SIZE 5

struct PCG32 {
    ulong state, inc;
};

uint pcg32(thread PCG32& rng) {
    ulong oldstate = rng.state;
    rng.state = oldstate * 6364136223846793005ul + (rng.inc | 1ul);
    uint xorshift = (uint) (((oldstate >> 18u) ^ oldstate) >> 27u);
    uint rotation = (uint) (oldstate >> 59u);
    return (xorshift >> rotation) | (xorshift << ((32u - rotation) & 31u));
}

void pcgSeed(thread PCG32& rng, ulong seed, ulong sequence) {
    rng.state = 0ul;
    rng.inc = (sequence << 1u) | 1u;
    pcg32(rng);
    rng.state += seed;
    pcg32(rng);
}

ulong hash(ulong x) {
    x += 0x9E3779B97F4A7C15ul;
    x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9ul;
    x = (x ^ (x >> 27)) * 0x94D049BB133111EBul;
    return x ^ (x >> 31);
}

float nextFloat(thread PCG32& rng) {
    return (float) pcg32(rng) * (1.0f / 4294967296.0f);
}

float3 randomVec(thread PCG32& rng) {
    return float3(nextFloat(rng), nextFloat(rng), nextFloat(rng));
}

float3 randomUV(thread PCG32& rng) {
    float u = nextFloat(rng);
    float v = nextFloat(rng);
    float theta = 2.0f * M_PI_F * u;
    float phi = acos(1.0f - 2.0f * v);
    float sinT = sin(theta);
    float cosT = cos(theta);
    float sinP = sin(phi);
    float cosP = cos(phi);
    return float3(sinP * cosT, sinP * sinT, cosP);
}

float3 randomOnHemisphere(thread PCG32& rng, thread const float3& normal) {
    float3 v = randomUV(rng);
    return dot(v, normal) > 0.0f ? v : -v;
}

float3 randomCosineHemisphere(thread PCG32& rng, thread const float3& normal) {
    float3 v = normalize(normal + randomUV(rng));
    if (dot(v, v) == 0.0f) {
        return normal;
    }
    return v;
}

float3 diffuse(thread PCG32& rng, thread const float3& normal) {
    return randomCosineHemisphere(rng, normal);
}

float reflectance(float cos, float n1, float n2) {
    float r0 = (n1 - n2) / (n1 + n2);
    r0 *= r0;
    return r0 + (1.0f - r0) * pow((1.0f - cos), 5);
}

float3 refract(thread PCG32& rng, thread const float3& dir, thread const float3& normal, float n1, float n2) {
    float cos = max(0.0f, -dot(dir, normal));
    float idx = n1 / n2;
    float r = 0.0f;
    if ((1.0f - cos * cos) * idx * idx > 1.0f) {
        r = 1.0f;
    } else {
        r = reflectance(cos, n1, n2);
    }
    if (r > nextFloat(rng)) {
        return dir + 2.0f * cos * normal;
    }
    float3 perp = idx * (dir + cos * normal);
    float3 para = -sqrt(fabs(1.0f - dot(perp, perp))) * normal;
    return perp + para;
}

struct Ray {
    float3 org, dir;
};

float3 rayAt(thread const Ray& ray, float t) {
    return ray.org + ray.dir * t;
}

struct HitRecord {
    float t;
    float3 p, n;
    bool exit;
};

void setHitNormal(thread HitRecord& hit, thread const Ray& ray, thread const float3& normal) {
    if (dot(ray.dir, normal) > 0) {
        hit.n = -normal;
        hit.exit = true;
    } else {
        hit.n = normal;
        hit.exit = false;
    }
}

enum ObjectType : int {
    SPHERE = 0,
    TRIANGLE = 1,
    QUAD = 2,
    AABB = 3,
    OBB = 4
};

struct Object {
    int type;
    float data[OBJECT_DATA_SIZE];
    int mIdx;
};

bool hitSphere(thread const Ray& ray, thread HitRecord& hit, float tMin, float tMax, thread const float data[OBJECT_DATA_SIZE]) {
    float3 center(data[0], data[1], data[2]);
    float radius = data[3];

    float3 oc = center - ray.org;

    float a = dot(ray.dir, ray.dir);
    float h = dot(ray.dir, oc);
    float c = dot(oc, oc) - radius * radius;
    float det = h * h - a * c;

    if (det < 0.0f) {
        return false;
    }

    float sqrtd = sqrt(det);
    float invA = 1.0f / a;

    float t = (h - sqrtd) * invA;
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

bool hitTriangle(thread const Ray& ray, thread HitRecord& hit, float tMin, float tMax, thread const float data[OBJECT_DATA_SIZE]) {
    float3 a(data[0], data[1], data[2]);
    float3 b(data[3], data[4], data[5]);
    float3 c(data[6], data[7], data[8]);
    float3 ab = b - a;
    float3 ac = c - a;

    float3 pvec = cross(ray.dir, ac);
    float det = dot(ab, pvec);

    if (fabs(det) <= EPSILON) {
        return false;
    }

    float invDet = 1.0f / det;

    float3 tvec = ray.org - a;
    float u = dot(tvec, pvec) * invDet;
    if (u < 0.0f || u > 1.0f) {
        return false;
    }

    float3 qvec = cross(tvec, ab);
    float v = dot(ray.dir, qvec) * invDet;
    if (v < 0.0f || u + v > 1.0f) {
        return false;
    }

    float t = dot(ac, qvec) * invDet;
    if (t < tMin || t > tMax) {
        return false;
    }

    hit.t = t;
    hit.p = rayAt(ray, t);
    setHitNormal(hit, ray, normalize(cross(ab, ac)));
    return true;
}

bool hitQuad(thread const Ray& ray, thread HitRecord& hit, float tMin, float tMax, thread const float data[OBJECT_DATA_SIZE]) {
    float3 center(data[0], data[1], data[2]);
    float3 u(data[3], data[4], data[5]);
    float3 v(data[6], data[7], data[8]);
    float3 origin =  center - u * 0.5f - v * 0.5f;
    float3 normal = normalize(cross(u, v));
    float uu = dot(u, u);
    float vv = dot(v, v);
    float uv = dot(u, v);
    float invPD = 1.0f / (uu * vv - uv * uv);

    float denom = dot(ray.dir, normal);

    if (fabs(denom) <= EPSILON) {
        return false;
    }

    float t = dot(center - ray.org, normal) / denom;
    if (t < tMin || t > tMax) {
        return false;
    }

    float3 p = rayAt(ray, t);
    float3 op = p - origin;
    float opu = dot(op, u);
    float opv = dot(op, v);
    float s = (opu * vv - opv * uv) * invPD;
    float w = (opv * uu - opu * uv) * invPD;
    if (s < 0.0f || s > 1.0f || w < 0.0f || w > 1.0f) {
        return false;
    }

    hit.t = t;
    hit.p = p;
    setHitNormal(hit, ray, normal);
    return true;
}

bool hitAABB(thread const Ray& ray, thread HitRecord& hit, float tMin, float tMax, thread const float data[OBJECT_DATA_SIZE]) {
    float3 bMin(data[0], data[1], data[2]);
    float3 bMax(data[3], data[4], data[5]);
    float3 halfs = (bMax - bMin) * 0.5f;
    float3 center = bMin + halfs;

    float3 ro = ray.org - center;
    float3 rd = ray.dir;

    float winding = 1.0f;
    float fx = fabs(ro.x) / halfs.x;
    float fy = fabs(ro.y) / halfs.y;
    float fz = fabs(ro.z) / halfs.z;
    if (fx < 1.0f && fy < 1.0f && fz < 1.0f) {
        winding = -1.0f;
    }

    float sgnX = -copysign(1.0f, rd.x);
    float sgnY = -copysign(1.0f, rd.y);
    float sgnZ = -copysign(1.0f, rd.z);

    float dx = (halfs.x * winding * sgnX - ro.x) / rd.x;
    float dy = (halfs.y * winding * sgnY - ro.y) / rd.y;
    float dz = (halfs.z * winding * sgnZ - ro.z) / rd.z;

    bool hitX = (dx >= 0.0f) & (fabs(ro.y + rd.y * dx) <= halfs.y) & (fabs(ro.z + rd.z * dx) <= halfs.z);
    bool hitY = (dy >= 0.0f) & (fabs(ro.z + rd.z * dy) <= halfs.z) & (fabs(ro.x + rd.x * dy) <= halfs.x);
    bool hitZ = (dz >= 0.0f) & (fabs(ro.x + rd.x * dz) <= halfs.x) & (fabs(ro.y + rd.y * dz) <= halfs.y);

    if (!hitX && !hitY && !hitZ) {
        return false;
    }

    float t = hitX ? dx : (hitY ? dy : dz);
    if (t < tMin || t > tMax) {
        return false;
    }

    hit.t = t;
    hit.p = rayAt(ray, t);
    hit.n = hitX ? float3(sgnX, 0.0f, 0.0f) : (hitY ? float3(0.0f, sgnY, 0.0f) : float3(0.0f, 0.0f, sgnZ));
    hit.exit = (winding < 0.0f);
    return true;
}

bool hitOBB(thread const Ray& ray, thread HitRecord& hit, float tMin, float tMax, thread const float data[OBJECT_DATA_SIZE]) {
    float3 bMin(data[0], data[1], data[2]);
    float3 bMax(data[3], data[4], data[5]);
    float3 halfs = (bMax - bMin) * 0.5f;
    float3 center = bMin + halfs;
    float3 u = float3(data[6], data[7], data[8]);
    float3 v = float3(data[9], data[10], data[11]);
    float3 w = cross(u, v);

    float3 ro = ray.org - center;
    float3 rd = ray.dir;
    ro = float3(dot(ro, u), dot(ro, v), dot(ro, w));
    rd = float3(dot(rd, u), dot(rd, v), dot(rd, w));

    float winding = 1.0f;
    float fx = fabs(ro.x) / halfs.x;
    float fy = fabs(ro.y) / halfs.y;
    float fz = fabs(ro.z) / halfs.z;
    if (fx < 1.0f && fy < 1.0f && fz < 1.0f) {
        winding = -1.0f;
    }

    float sgnX = -copysign(1.0f, rd.x);
    float sgnY = -copysign(1.0f, rd.y);
    float sgnZ = -copysign(1.0f, rd.z);

    float dx = (halfs.x * winding * sgnX - ro.x) / rd.x;
    float dy = (halfs.y * winding * sgnY - ro.y) / rd.y;
    float dz = (halfs.z * winding * sgnZ - ro.z) / rd.z;

    bool hitX = (dx >= 0.0f) & (fabs(ro.y + rd.y * dx) <= halfs.y) & (fabs(ro.z + rd.z * dx) <= halfs.z);
    bool hitY = (dy >= 0.0f) & (fabs(ro.z + rd.z * dy) <= halfs.z) & (fabs(ro.x + rd.x * dy) <= halfs.x);
    bool hitZ = (dz >= 0.0f) & (fabs(ro.x + rd.x * dz) <= halfs.x) & (fabs(ro.y + rd.y * dz) <= halfs.y);

    if (!hitX && !hitY && !hitZ) {
        return false;
    }

    float t = hitX ? dx : (hitY ? dy : dz);
    if (t < tMin || t > tMax) {
        return false;
    }

    float3 localN = hitX ? float3(sgnX, 0.0f, 0.0f) : (hitY ? float3(0.0f, sgnY, 0.0f) : float3(0.0f, 0.0f, sgnZ));

    hit.t = t;
    hit.p = rayAt(ray, t);
    hit.n = u * localN.x + v * localN.y + w * localN.z;
    hit.exit = (winding < 0.0f);
    return true;
}

bool hitObject(thread const Ray& ray, thread HitRecord& hit, float tMin, float tMax, thread const Object& obj) {
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

enum MaterialType : int {
    DIFFUSE = 0,
    METAL = 1,
    DIELECTRIC = 2,
    EMISSIVE = 3
};

struct Material {
    int type;
    float data[MATERIAL_DATA_SIZE];
};

struct ScatterResult {
    Ray ray;
    float3 albedo;
    bool scattered;
};

ScatterResult scatterDiffuse(thread PCG32& rng, thread const Ray& ray, thread const HitRecord& hit, thread const float data[MATERIAL_DATA_SIZE]) {
    ScatterResult res;
    res.albedo = float3(data[0], data[1], data[2]);
    res.scattered = true;
    res.ray.dir = diffuse(rng, hit.n);
    res.ray.org = hit.p + res.ray.dir * 1e-4f;
    return res;
}

ScatterResult scatterMetal(thread PCG32& rng, thread const Ray& ray, thread const HitRecord& hit, thread const float data[MATERIAL_DATA_SIZE]) {
    ScatterResult res;
    res.albedo = float3(data[0], data[1], data[2]);
    res.scattered = true;
    if ((1.0f - data[4]) * abs(dot(ray.dir, hit.n)) < nextFloat(rng)) {
        res.ray.dir = normalize(reflect(ray.dir, hit.n) + randomUV(rng) * data[3]);
    } else {
        res.ray.dir = diffuse(rng, hit.n);
    }
    res.ray.org = hit.p + res.ray.dir * 1e-4f;
    return res;
}

ScatterResult scatterDielectric(thread PCG32& rng, thread const Ray& ray, thread const HitRecord& hit, thread const float data[MATERIAL_DATA_SIZE]) {
    ScatterResult res;
    res.albedo = float3(data[0], data[1], data[2]);
    res.scattered = true;
    float n1 = 1.0f, n2 = data[3];
    if (hit.exit) {
        float tmp = n1;
        n1 = n2;
        n2 = tmp;
    }
    res.ray.dir = normalize(refract(rng, ray.dir, hit.n, n1, n2) + randomUV(rng) * data[4]);
    res.ray.org = hit.p + res.ray.dir * 1e-4f;
    return res;
}

ScatterResult scatterEmissive(thread PCG32& rng, thread const Ray& ray, thread const HitRecord& hit, thread const float data[MATERIAL_DATA_SIZE]) {
    return scatterDiffuse(rng, ray, hit, data);
}

ScatterResult scatterMaterial(thread PCG32& rng, thread const Ray& ray, thread const HitRecord& hit, thread const Material& material) {
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

struct RenderParams {
    int screenW, screenH, samplesPerPixel, maxBounces;
    packed_float3 camPos, pixelOrigin, pixelDeltaW, pixelDeltaH;
};

struct SceneData {
    ulong oCount, mCount;
};

kernel void render(
                   device const RenderParams* p [[buffer(0)]],
                   device const Object* objects [[buffer(1)]],
                   device const Material* materials [[buffer(2)]],
                   device const SceneData* sceneData [[buffer(3)]],
                   device packed_float3* pixels [[buffer(4)]],
                   uint2 id [[thread_position_in_grid]]
) {
    uint width = (uint) p->screenW;
    uint height = (uint) p->screenH;
    if (id.x >= width || id.y >= height) {
        return;
    }

    PCG32 rng;
    float3 pixel = p->pixelOrigin + p->pixelDeltaW * id.x + p->pixelDeltaH * id.y;

    Ray ray;

    float3 totalColor(0.0f);

    for (int i = 0; i < p->samplesPerPixel; i++) {
        ulong seed = hash(i);
        ulong sequence = hash(id.y * width + id.x);
        pcgSeed(rng, seed, sequence);
        float3 jitter = p->pixelDeltaW * (nextFloat(rng) - 0.5f) + p->pixelDeltaH * (nextFloat(rng) - 0.5f);

        ray.org = p->camPos;
        ray.dir = normalize(pixel + jitter - p->camPos);

        HitRecord hit;
        ScatterResult sres;

        float3 radiance(0.0f);
        float3 throughput(1.0f);
        float3 emission(0.0f);

        for (int j = 0; j < p->maxBounces; j++) {
            int mIdx = -1;
            float closestT = INFINITY;
            for (ulong o = 0; o < sceneData->oCount; o++) {
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

            emission = float3(0.0f);
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
