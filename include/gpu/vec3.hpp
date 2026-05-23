#ifndef GPU_VEC3_HPP
#define GPU_VEC3_HPP

// Std includes
#include <cmath>

// Local includes
#include <util/types.h>
#include <util/epsilon.h>

#ifdef __CUDACC__
#define CUDA_CALLABLE __host__ __device__
#else
#define CUDA_CALLABLE
#endif

namespace gpu {

struct vec3 {
    float32 x, y, z;

    CUDA_CALLABLE vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    CUDA_CALLABLE vec3(float32 f) : x(f), y(f), z(f) {}
    CUDA_CALLABLE vec3(float32 x, float32 y, float32 z) : x(x), y(y), z(z) {}
};

// Single Vector

CUDA_CALLABLE inline vec3 operator-(const vec3& v) {
    return vec3(-v.x, -v.y, -v.z);
}
CUDA_CALLABLE inline vec3 inv(const vec3& v) {
    return vec3(1.0f / v.x, 1.0f / v.y, 1.0f / v.z);
}

// Vector-Vector

CUDA_CALLABLE inline vec3 operator+(const vec3& a, const vec3& b) {
    return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}
CUDA_CALLABLE inline vec3 operator-(const vec3& a, const vec3& b) {
    return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}
CUDA_CALLABLE inline vec3 operator*(const vec3& a, const vec3& b) {
    return vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}
CUDA_CALLABLE inline vec3 operator/(const vec3& a, const vec3& b) {
    return vec3(a.x / b.x, a.y / b.y, a.z / b.z);
}

// Vector-Scalar

CUDA_CALLABLE inline vec3 operator+(const vec3& v, float32 f) {
    return vec3(v.x + f, v.y + f, v.z + f);
}
CUDA_CALLABLE inline vec3 operator+(float32 f, const vec3& v) {
    return vec3(f + v.x, f + v.y, f + v.z);
}
CUDA_CALLABLE inline vec3 operator-(const vec3& v, float32 f) {
    return vec3(v.x - f, v.y - f, v.z - f);
}
CUDA_CALLABLE inline vec3 operator-(float32 f, const vec3& v) {
    return vec3(f - v.x, f - v.y, f - v.z);
}
CUDA_CALLABLE inline vec3 operator*(const vec3& v, float32 f) {
    return vec3(v.x * f, v.y * f, v.z * f);
}
CUDA_CALLABLE inline vec3 operator*(float32 f, const vec3& v) {
    return vec3(f * v.x, f * v.y, f * v.z);
}
CUDA_CALLABLE inline vec3 operator/(const vec3& v, float32 f) {
    return vec3(v.x / f, v.y / f, v.z / f);
}
CUDA_CALLABLE inline vec3 operator/(float32 f, const vec3& v) {
    return vec3(f / v.x, f / v.y, f / v.z);
}

// Vector specific

CUDA_CALLABLE inline float32 dot(const vec3& a, const vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
CUDA_CALLABLE inline vec3 cross(const vec3& a, const vec3& b) {
    return vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}
CUDA_CALLABLE inline float32 lenSq(const vec3& v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}
CUDA_CALLABLE inline float32 len(const vec3& v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
CUDA_CALLABLE inline vec3 normalize(const vec3& v) {
    float32 l = len(v);
    if (l <= EPSILON) {
        return vec3(0.0f);
    }
    return vec3(v.x / l, v.y / l, v.z / l);
}

// What's their name

CUDA_CALLABLE inline vec3& operator+=(vec3& a, const vec3& b) {
    a = a + b;
    return a;
}
CUDA_CALLABLE inline vec3& operator-=(vec3& a, const vec3& b) {
    a = a - b;
    return a;
}
CUDA_CALLABLE inline vec3& operator*=(vec3& a, const vec3& b) {
    a = a * b;
    return a;
}
CUDA_CALLABLE inline vec3& operator/=(vec3& a, const vec3& b) {
    a = a / b;
    return a;
}
CUDA_CALLABLE inline vec3& operator+=(vec3& v, float32 f) {
    v = v + f;
    return v;
}
CUDA_CALLABLE inline vec3& operator-=(vec3& v, float32 f) {
    v = v - f;
    return v;
}
CUDA_CALLABLE inline vec3& operator*=(vec3& v, float32 f) {
    v = v * f;
    return v;
}
CUDA_CALLABLE inline vec3& operator/=(vec3& v, float32 f) {
    v = v / f;
    return v;
}

} // namespace gpu

#endif // GPU_VEC3_HPP
