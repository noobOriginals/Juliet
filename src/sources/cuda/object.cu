#include <cuda/object.cuh>

namespace cuda {

__device__ bool hitSphere(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[core::OBJECT_DATA_SIZE]) {
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

__device__ bool hitTriangle(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[core::OBJECT_DATA_SIZE]) {
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

__device__ bool hitQuad(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[core::OBJECT_DATA_SIZE]) {
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

__device__ bool hitAABB(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[core::OBJECT_DATA_SIZE]) {
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

__device__ bool hitOBB(const Ray& ray, HitRecord& hit, float32 tMin, float32 tMax, const float32 data[core::OBJECT_DATA_SIZE]) {
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
    case core::SPHERE: {
        return hitSphere(ray, hit, tMin, tMax, obj.data);
    }

    case core::TRIANGLE: {
        return hitTriangle(ray, hit, tMin, tMax, obj.data);
    }

    case core::QUAD: {
        return hitQuad(ray, hit, tMin, tMax, obj.data);
    }

    case core::AABB: {
        return hitAABB(ray, hit, tMin, tMax, obj.data);
    }

    case core::OBB: {
        return hitOBB(ray, hit, tMin, tMax, obj.data);
    }

    default: {
        return false;
    }
    }
}

} // namespace cuda
