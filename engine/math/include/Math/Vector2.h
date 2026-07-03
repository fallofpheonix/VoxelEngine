#pragma once

#include "Foundation/Assert.h"
#include "Math/Constants.h"

#include <cmath>

namespace Engine {

struct Vector2 {
    float x = 0.0f;
    float y = 0.0f;

    Vector2() = default;
    Vector2(float x_, float y_) : x(x_), y(y_) {}

    Vector2 operator+(const Vector2& rhs) const { return {x + rhs.x, y + rhs.y}; }
    Vector2 operator-(const Vector2& rhs) const { return {x - rhs.x, y - rhs.y}; }
    Vector2 operator-() const { return {-x, -y}; }
    Vector2 operator*(float s) const { return {x * s, y * s}; }
    Vector2 operator/(float s) const {
        ENGINE_ASSERT(s != 0.0f, "Vector2 division by zero");
        return {x / s, y / s};
    }

    Vector2& operator+=(const Vector2& rhs) { x += rhs.x; y += rhs.y; return *this; }
    Vector2& operator-=(const Vector2& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
    Vector2& operator*=(float s) { x *= s; y *= s; return *this; }

    // No operator==: exact float comparison is almost never correct for a
    // computed vector. Use NearlyEqual. See
    // docs/decisions/0007-floating-point-equality.md.
    bool NearlyEqual(const Vector2& rhs, float epsilon = kEpsilon) const {
        return Engine::NearlyEqual(x, rhs.x, epsilon) && Engine::NearlyEqual(y, rhs.y, epsilon);
    }

    float Dot(const Vector2& rhs) const { return x * rhs.x + y * rhs.y; }
    float LengthSquared() const { return Dot(*this); }
    float Length() const { return std::sqrt(LengthSquared()); }

    Vector2 Normalized() const {
        const float len = Length();
        ENGINE_ASSERT(len > 0.0f, "Cannot normalize a zero-length Vector2");
        return {x / len, y / len};
    }
};

inline Vector2 operator*(float s, const Vector2& v) { return v * s; }

inline Vector2 Lerp(const Vector2& a, const Vector2& b, float t) {
    return {Lerp(a.x, b.x, t), Lerp(a.y, b.y, t)};
}

} // namespace Engine
