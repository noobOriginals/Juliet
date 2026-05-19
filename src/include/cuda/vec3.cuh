#ifndef CUDA_VEC3_CUH
#define CUDA_VEC3_CUH

// Std includes
#include <cuda_runtime.h>

// Local includes
#include <util/types.h>
#include <util/epsilon.h>

namespace cuda {

struct vec3 {
    float32 x, y, z;

    __host__ __device__ vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    __host__ __device__ vec3(float32 f) : x(f), y(f), z(f) {}
    __host__ __device__ vec3(float32 x, float32 y, float32 z) : x(x), y(y), z(z) {}
};

// Single Vector

inline __device__ vec3 operator-(const vec3& v) {
    return vec3(-v.x, -v.y, -v.z);
}
inline __device__ vec3 inv(const vec3& v) {
    return vec3(1.0f / v.x, 1.0f / v.y, 1.0f / v.z);
}

// Vector-Vector

inline __device__ vec3 operator+(const vec3& a, const vec3& b) {
    return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}
inline __device__ vec3 operator-(const vec3& a, const vec3& b) {
    return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}
inline __device__ vec3 operator*(const vec3& a, const vec3& b) {
    return vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}
inline __device__ vec3 operator/(const vec3& a, const vec3& b) {
    return vec3(a.x / b.x, a.y / b.y, a.z / b.z);
}

// Vector-Scalar

inline __device__ vec3 operator+(const vec3& v, float32 f) {
    return vec3(v.x + f, v.y + f, v.z + f);
}
inline __device__ vec3 operator+(float32 f, const vec3& v) {
    return vec3(f + v.x, f + v.y, f + v.z);
}
inline __device__ vec3 operator-(const vec3& v, float32 f) {
    return vec3(v.x - f, v.y - f, v.z - f);
}
inline __device__ vec3 operator-(float32 f, const vec3& v) {
    return vec3(f - v.x, f - v.y, f - v.z);
}
inline __device__ vec3 operator*(const vec3& v, float32 f) {
    return vec3(v.x * f, v.y * f, v.z * f);
}
inline __device__ vec3 operator*(float32 f, const vec3& v) {
    return vec3(f * v.x, f * v.y, f * v.z);
}
inline __device__ vec3 operator/(const vec3& v, float32 f) {
    return vec3(v.x / f, v.y / f, v.z / f);
}
inline __device__ vec3 operator/(float32 f, const vec3& v) {
    return vec3(f / v.x, f / v.y, f / v.z);
}

// Vector specific

inline __device__ float32 dot(const vec3& a, const vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
inline __device__ vec3 cross(const vec3& a, const vec3& b) {
    return vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}
inline __device__ float32 lenSq(const vec3& v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}
inline __device__ float32 len(const vec3& v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
inline __device__ vec3 normalize(const vec3& v) {
    float32 l = len(v);
    if (l <= EPSILON) {
        return vec3(0.0f);
    }
    return vec3(v.x / l, v.y / l, v.z / l);
}

// What's their name

inline __device__ vec3& operator+=(vec3& a, const vec3& b) {
    a = a + b;
    return a;
}
inline __device__ vec3& operator-=(vec3& a, const vec3& b) {
    a = a - b;
    return a;
}
inline __device__ vec3& operator*=(vec3& a, const vec3& b) {
    a = a * b;
    return a;
}
inline __device__ vec3& operator/=(vec3& a, const vec3& b) {
    a = a / b;
    return a;
}
inline __device__ vec3& operator+=(vec3& v, float32 f) {
    v = v + f;
    return v;
}
inline __device__ vec3& operator-=(vec3& v, float32 f) {
    v = v - f;
    return v;
}
inline __device__ vec3& operator*=(vec3& v, float32 f) {
    v = v * f;
    return v;
}
inline __device__ vec3& operator/=(vec3& v, float32 f) {
    v = v / f;
    return v;
}

} // namespace cuda

#endif // CUDA_VEC3_CUH
