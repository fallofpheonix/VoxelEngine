#pragma once

#include "Foundation/Assert.h"
#include "Math/Constants.h"
#include "Math/Vector3.h"

#include <cmath>

namespace Engine {

struct Vector4 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;

    Vector4() = default;
    Vector4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}
    explicit Vector4(const Vector3& v, float w_) : x(v.x), y(v.y), z(v.z), w(w_) {}

    Vector3 XYZ() const { return {x, y, z}; }

    Vector4 operator+(const Vector4& rhs) const { return {x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w}; }
    Vector4 operator-(const Vector4& rhs) const { return {x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w}; }
    Vector4 operator*(float s) const { return {x * s, y * s, z * s, w * s}; }
    Vector4 operator/(float s) const {
        ENGINE_ASSERT(s != 0.0f, "Vector4 division by zero");
        return {x / s, y / s, z / s, w / s};
    }

    // No operator==: exact float comparison is almost never correct for a
    // computed vector. Use NearlyEqual. See
    // docs/decisions/0007-floating-point-equality.md.
    bool NearlyEqual(const Vector4& rhs, float epsilon = kEpsilon) const {
        return Engine::NearlyEqual(x, rhs.x, epsilon) && Engine::NearlyEqual(y, rhs.y, epsilon) &&
               Engine::NearlyEqual(z, rhs.z, epsilon) && Engine::NearlyEqual(w, rhs.w, epsilon);
    }

    float Dot(const Vector4& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w; }
    float LengthSquared() const { return Dot(*this); }
    float Length() const { return std::sqrt(LengthSquared()); }
};

inline Vector4 operator*(float s, const Vector4& v) { return v * s; }

inline Vector4 Lerp(const Vector4& a, const Vector4& b, float t) {
    return {Lerp(a.x, b.x, t), Lerp(a.y, b.y, t), Lerp(a.z, b.z, t), Lerp(a.w, b.w, t)};
}

} // namespace Engine
