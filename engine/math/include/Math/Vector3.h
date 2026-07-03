#pragma once

#include "Foundation/Assert.h"
#include "Math/Constants.h"

#include <cmath>

namespace Engine {

struct Vector3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vector3() = default;
    Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    Vector3 operator+(const Vector3& rhs) const { return {x + rhs.x, y + rhs.y, z + rhs.z}; }
    Vector3 operator-(const Vector3& rhs) const { return {x - rhs.x, y - rhs.y, z - rhs.z}; }
    Vector3 operator-() const { return {-x, -y, -z}; }
    Vector3 operator*(float s) const { return {x * s, y * s, z * s}; }
    Vector3 operator/(float s) const {
        ENGINE_ASSERT(s != 0.0f, "Vector3 division by zero");
        return {x / s, y / s, z / s};
    }

    Vector3& operator+=(const Vector3& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
    Vector3& operator-=(const Vector3& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
    Vector3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }

    // No operator==: exact float comparison is almost never correct for a
    // computed vector. Use NearlyEqual. See
    // docs/decisions/0007-floating-point-equality.md.
    bool NearlyEqual(const Vector3& rhs, float epsilon = kEpsilon) const {
        return Engine::NearlyEqual(x, rhs.x, epsilon) && Engine::NearlyEqual(y, rhs.y, epsilon) &&
               Engine::NearlyEqual(z, rhs.z, epsilon);
    }

    float Dot(const Vector3& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }

    Vector3 Cross(const Vector3& rhs) const {
        return {
            y * rhs.z - z * rhs.y,
            z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x,
        };
    }

    float LengthSquared() const { return Dot(*this); }
    float Length() const { return std::sqrt(LengthSquared()); }

    Vector3 Normalized() const {
        const float len = Length();
        ENGINE_ASSERT(len > 0.0f, "Cannot normalize a zero-length Vector3");
        return {x / len, y / len, z / len};
    }

    static Vector3 Zero() { return {0.0f, 0.0f, 0.0f}; }
    static Vector3 One() { return {1.0f, 1.0f, 1.0f}; }
    static Vector3 UnitX() { return {1.0f, 0.0f, 0.0f}; }
    static Vector3 UnitY() { return {0.0f, 1.0f, 0.0f}; }
    static Vector3 UnitZ() { return {0.0f, 0.0f, 1.0f}; }

    // Fixed world-space direction convention -- see engine/math/README.md.
    // Right-handed: Right x Up = Back (== -Forward), consistent with an
    // OpenGL-style camera that looks down -Z by default.
    static Vector3 Right()   { return {1.0f, 0.0f, 0.0f}; }
    static Vector3 Left()    { return {-1.0f, 0.0f, 0.0f}; }
    static Vector3 Up()      { return {0.0f, 1.0f, 0.0f}; }
    static Vector3 Down()    { return {0.0f, -1.0f, 0.0f}; }
    static Vector3 Forward() { return {0.0f, 0.0f, -1.0f}; }
    static Vector3 Back()    { return {0.0f, 0.0f, 1.0f}; }
};

inline Vector3 operator*(float s, const Vector3& v) { return v * s; }

inline Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
    return {Lerp(a.x, b.x, t), Lerp(a.y, b.y, t), Lerp(a.z, b.z, t)};
}

} // namespace Engine
