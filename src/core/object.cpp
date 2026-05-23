#include <core/object.hpp>

// Std includes
#include <cmath>

// Local includes
#include <util/epsilon.h>

namespace core {

Object makeSphere(const glm::vec3& center, float32 radius, int32 materialIdx) {
    Object obj;
    obj.type = SPHERE;
    obj.materialIdx = materialIdx;
    obj.data[0] = center.x;
    obj.data[1] = center.y;
    obj.data[2] = center.z;
    obj.data[3] = radius;
    return obj;
}

Object makeTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, int32 materialIdx) {
    Object obj;
    obj.type = TRIANGLE;
    obj.materialIdx = materialIdx;
    obj.data[0] = a.x;
    obj.data[1] = a.y;
    obj.data[2] = a.z;
    obj.data[3] = b.x;
    obj.data[4] = b.y;
    obj.data[5] = b.z;
    obj.data[6] = c.x;
    obj.data[7] = c.y;
    obj.data[8] = c.z;
    return obj;
}

Object makeQuad(const glm::vec3& center, const glm::vec3& u, const glm::vec3& v, int32 materialIdx) {
    Object obj;
    obj.type = QUAD;
    obj.materialIdx = materialIdx;
    obj.data[0] = center.x;
    obj.data[1] = center.y;
    obj.data[2] = center.z;
    obj.data[3] = u.x;
    obj.data[4] = u.y;
    obj.data[5] = u.z;
    obj.data[6] = v.x;
    obj.data[7] = v.y;
    obj.data[8] = v.z;
    return obj;
}

Object makeAABB(const glm::vec3& bMin, const glm::vec3& bMax, int32 materialIdx) {
    Object obj;
    obj.type = AABB;
    obj.materialIdx = materialIdx;
    obj.data[0] = bMin.x;
    obj.data[1] = bMin.y;
    obj.data[2] = bMin.z;
    obj.data[3] = bMax.x;
    obj.data[4] = bMax.y;
    obj.data[5] = bMax.z;
    return obj;
}

Object makeOBB(const glm::vec3& bMin, const glm::vec3& bMax, const glm::vec3& u, const glm::vec3& v, int32 materialIdx) {
    glm::vec3 nu = glm::normalize(u);
    glm::vec3 up = glm::cross(u, v);
    glm::vec3 nv = glm::normalize(glm::cross(up, u));
    Object obj;
    obj.type = OBB;
    obj.materialIdx = materialIdx;
    obj.data[0] = bMin.x;
    obj.data[1] = bMin.y;
    obj.data[2] = bMin.z;
    obj.data[3] = bMax.x;
    obj.data[4] = bMax.y;
    obj.data[5] = bMax.z;
    obj.data[6] = nu.x;
    obj.data[7] = nu.y;
    obj.data[8] = nu.z;
    obj.data[9] = nv.x;
    obj.data[10] = nv.y;
    obj.data[11] = nv.z;
    return obj;
}

bool hitSphere(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[OBJECT_DATA_SIZE]) {
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

bool hitTriangle(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[OBJECT_DATA_SIZE]) {
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

bool hitQuad(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[OBJECT_DATA_SIZE]) {
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

bool hitAABB(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[OBJECT_DATA_SIZE]) {
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

bool hitOBB(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[OBJECT_DATA_SIZE]) {
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

bool hitObject(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const Object& obj) {
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

}  // namespace core
