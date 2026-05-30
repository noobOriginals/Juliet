#include "core/cpu/object_ops.hpp"

// Local includes
#include "util/epsilon.h"

namespace core {

void makeSphere(float32 data[OBJECT_DATA_SIZE], const glm::vec3& center, float32 radius) {
    data[0] = center.x;
    data[1] = center.y;
    data[2] = center.z;
    data[3] = radius;
}

void makeTriangle(float32 data[OBJECT_DATA_SIZE], const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
    data[0] = a.x;
    data[1] = a.y;
    data[2] = a.z;
    data[3] = b.x;
    data[4] = b.y;
    data[5] = b.z;
    data[6] = c.x;
    data[7] = c.y;
    data[8] = c.z;
}

void makeQuad(float32 data[OBJECT_DATA_SIZE], const glm::vec3& center, const glm::vec3& u, const glm::vec3& v) {
    data[0] = center.x;
    data[1] = center.y;
    data[2] = center.z;
    data[3] = u.x;
    data[4] = u.y;
    data[5] = u.z;
    data[6] = v.x;
    data[7] = v.y;
    data[8] = v.z;
}

void makeAABB(float32 data[OBJECT_DATA_SIZE], const glm::vec3& bMin, const glm::vec3& bMax) {
    data[0] = bMin.x;
    data[1] = bMin.y;
    data[2] = bMin.z;
    data[3] = bMax.x;
    data[4] = bMax.y;
    data[5] = bMax.z;
}

void makeOBB(float32 data[OBJECT_DATA_SIZE], const glm::vec3& bMin, const glm::vec3& bMax, const glm::vec3& u, const glm::vec3& v) {
    glm::vec3 nu = glm::normalize(u);
    glm::vec3 up = glm::cross(u, v);
    glm::vec3 nv = glm::normalize(glm::cross(up, u));
    data[0] = bMin.x;
    data[1] = bMin.y;
    data[2] = bMin.z;
    data[3] = bMax.x;
    data[4] = bMax.y;
    data[5] = bMax.z;
    data[6] = nu.x;
    data[7] = nu.y;
    data[8] = nu.z;
    data[9] = nv.x;
    data[10] = nv.y;
    data[11] = nv.z;
}

bool hitSphere(float32 data[OBJECT_DATA_SIZE], const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax) {
    glm::vec3 center(data[0], data[1], data[2]);
    float32 radius = data[3];

    glm::vec3 oc = center - ray.org;

    float32 a = glm::dot(ray.dir, ray.dir);
    float32 h = glm::dot(ray.dir, oc);
    float32 c = glm::dot(oc, oc) - radius * radius;
    float32 det = h * h - a * c;

    if (det < 0.0f) {
        return false;
    }

    float32 sqrtd = std::sqrt(det);
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

bool hitTriangle(float32 data[OBJECT_DATA_SIZE], const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax) {
    glm::vec3 a(data[0], data[1], data[2]);
    glm::vec3 b(data[3], data[4], data[5]);
    glm::vec3 c(data[6], data[7], data[8]);
    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;

    glm::vec3 pvec = glm::cross(ray.dir, ac);
    float32 det = glm::dot(ab, pvec);

    if (std::fabs(det) <= EPSILON) {
        return false;
    }

    float32 invDet = 1.0f / det;

    glm::vec3 tvec = ray.org - a;
    float32 u = glm::dot(tvec, pvec) * invDet;
    if (u < 0.0f || u > 1.0f) {
        return false;
    }

    glm::vec3 qvec = glm::cross(tvec, ab);
    float32 v = glm::dot(ray.dir, qvec) * invDet;
    if (v < 0.0f || u + v > 1.0f) {
        return false;
    }

    float32 t = glm::dot(ac, qvec) * invDet;
    if (t < tMin || t > tMax) {
        return false;
    }

    hit.t = t;
    hit.p = rayAt(ray, t);
    setHitNormal(hit, ray, glm::normalize(glm::cross(ab, ac)));
    return true;
}

bool hitQuad(float32 data[OBJECT_DATA_SIZE], const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax) {
    glm::vec3 center(data[0], data[1], data[2]);
    glm::vec3 u(data[3], data[4], data[5]);
    glm::vec3 v(data[6], data[7], data[8]);
    glm::vec3 origin =  center - u * 0.5f - v * 0.5f;
    glm::vec3 normal = glm::normalize(glm::cross(u, v));
    float32 uu = glm::dot(u, u);
    float32 vv = glm::dot(v, v);
    float32 uv = glm::dot(u, v);
    float32 invPD = 1.0f / (uu * vv - uv * uv);

    float32 denom = glm::dot(ray.dir, normal);

    if (std::fabs(denom) <= EPSILON) {
        return false;
    }

    float32 t = glm::dot(center - ray.org, normal) / denom;
    if (t < tMin || t > tMax) {
        return false;
    }

    glm::vec3 p = rayAt(ray, t);
    glm::vec3 op = p - origin;
    float32 opu = glm::dot(op, u);
    float32 opv = glm::dot(op, v);
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

bool hitAABB(float32 data[OBJECT_DATA_SIZE], const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax) {
    glm::vec3 bMin(data[0], data[1], data[2]);
    glm::vec3 bMax(data[3], data[4], data[5]);
    glm::vec3 halfs = (bMax - bMin) * 0.5f;
    glm::vec3 center = bMin + halfs;

    glm::vec3 ro = ray.org - center;
    glm::vec3 rd = ray.dir;

    float32 winding = 1.0f;
    float32 fx = std::fabs(ro.x) / halfs.x;
    float32 fy = std::fabs(ro.y) / halfs.y;
    float32 fz = std::fabs(ro.z) / halfs.z;
    if (fx < 1.0f && fy < 1.0f && fz < 1.0f) {
        winding = -1.0f;
    }

    float32 sgnX = -std::copysign(1.0f, rd.x);
    float32 sgnY = -std::copysign(1.0f, rd.y);
    float32 sgnZ = -std::copysign(1.0f, rd.z);

    float32 dx = (halfs.x * winding * sgnX - ro.x) / rd.x;
    float32 dy = (halfs.y * winding * sgnY - ro.y) / rd.y;
    float32 dz = (halfs.z * winding * sgnZ - ro.z) / rd.z;

    bool hitX = (dx >= 0.0f) & (std::fabs(ro.y + rd.y * dx) <= halfs.y) & (std::fabs(ro.z + rd.z * dx) <= halfs.z);
    bool hitY = (dy >= 0.0f) & (std::fabs(ro.z + rd.z * dy) <= halfs.z) & (std::fabs(ro.x + rd.x * dy) <= halfs.x);
    bool hitZ = (dz >= 0.0f) & (std::fabs(ro.x + rd.x * dz) <= halfs.x) & (std::fabs(ro.y + rd.y * dz) <= halfs.y);

    if (!hitX && !hitY && !hitZ) {
        return false;
    }

    float32 t = hitX ? dx : (hitY ? dy : dz);
    if (t < tMin || t > tMax) {
        return false;
    }

    hit.t = t;
    hit.p = rayAt(ray, t);
    hit.n = hitX ? glm::vec3(sgnX, 0.0f, 0.0f) : (hitY ? glm::vec3(0.0f, sgnY, 0.0f) : glm::vec3(0.0f, 0.0f, sgnZ));
    hit.exit = (winding < 0.0f);
    return true;
}

bool hitOBB(float32 data[OBJECT_DATA_SIZE], const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax) {
    glm::vec3 bMin(data[0], data[1], data[2]);
    glm::vec3 bMax(data[3], data[4], data[5]);
    glm::vec3 halfs = (bMax - bMin) * 0.5f;
    glm::vec3 center = bMin + halfs;
    glm::vec3 u = glm::vec3(data[6], data[7], data[8]);
    glm::vec3 v = glm::vec3(data[9], data[10], data[11]);
    glm::vec3 w = glm::cross(u, v);

    glm::vec3 ro = ray.org - center;
    glm::vec3 rd = ray.dir;
    ro = glm::vec3(glm::dot(ro, u), glm::dot(ro, v), glm::dot(ro, w));
    rd = glm::vec3(glm::dot(rd, u), glm::dot(rd, v), glm::dot(rd, w));

    float32 winding = 1.0f;
    float32 fx = std::fabs(ro.x) / halfs.x;
    float32 fy = std::fabs(ro.y) / halfs.y;
    float32 fz = std::fabs(ro.z) / halfs.z;
    if (fx < 1.0f && fy < 1.0f && fz < 1.0f) {
        winding = -1.0f;
    }

    float32 sgnX = -std::copysign(1.0f, rd.x);
    float32 sgnY = -std::copysign(1.0f, rd.y);
    float32 sgnZ = -std::copysign(1.0f, rd.z);

    float32 dx = (halfs.x * winding * sgnX - ro.x) / rd.x;
    float32 dy = (halfs.y * winding * sgnY - ro.y) / rd.y;
    float32 dz = (halfs.z * winding * sgnZ - ro.z) / rd.z;

    bool hitX = (dx >= 0.0f) & (std::fabs(ro.y + rd.y * dx) <= halfs.y) & (std::fabs(ro.z + rd.z * dx) <= halfs.z);
    bool hitY = (dy >= 0.0f) & (std::fabs(ro.z + rd.z * dy) <= halfs.z) & (std::fabs(ro.x + rd.x * dy) <= halfs.x);
    bool hitZ = (dz >= 0.0f) & (std::fabs(ro.x + rd.x * dz) <= halfs.x) & (std::fabs(ro.y + rd.y * dz) <= halfs.y);

    if (!hitX && !hitY && !hitZ) {
        return false;
    }

    float32 t = hitX ? dx : (hitY ? dy : dz);
    if (t < tMin || t > tMax) {
        return false;
    }

    glm::vec3 localN = hitX ? glm::vec3(sgnX, 0.0f, 0.0f) : (hitY ? glm::vec3(0.0f, sgnY, 0.0f) : glm::vec3(0.0f, 0.0f, sgnZ));

    hit.t = t;
    hit.p = rayAt(ray, t);
    hit.n = u * localN.x + v * localN.y + w * localN.z;
    hit.exit = (winding < 0.0f);
    return true;
}

bool hitObject(int32 type, float32 data[OBJECT_DATA_SIZE], const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax) {
    switch (type) {
    case SPHERE: return hitSphere(data, ray, hit, tMin, tMax);
    case TRIANGLE: return hitTriangle(data, ray, hit, tMin, tMax);
    case QUAD: return hitQuad(data, ray, hit, tMin, tMax);
    case AABB: return hitAABB(data, ray, hit, tMin, tMax);
    case OBB: return hitOBB(data, ray, hit, tMin, tMax);
    default: return false;
    }
}

} // namespace core
