#pragma once

#include <cmath>
#include <limits>

namespace Engine {

constexpr float kPi = 3.14159265358979323846f;
constexpr float kTwoPi = 2.0f * kPi;
constexpr float kHalfPi = kPi / 2.0f;

// Default tolerance for NearlyEqual() across the math library. Exact
// floating-point equality (0.1f + 0.2f != 0.3f) is almost never the right
// comparison for computed vectors/matrices/quaternions -- see
// docs/decisions/0007-floating-point-equality.md.
constexpr float kEpsilon = 1e-5f;

constexpr float kInfinity = std::numeric_limits<float>::infinity();
constexpr float kNegativeInfinity = -std::numeric_limits<float>::infinity();

constexpr float DegToRad(float degrees) {
    return degrees * (kPi / 180.0f);
}

constexpr float RadToDeg(float radians) {
    return radians * (180.0f / kPi);
}

constexpr float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

constexpr float Clamp(float value, float min, float max) {
    return value < min ? min : (value > max ? max : value);
}

inline bool NearlyEqual(float a, float b, float epsilon = kEpsilon) {
    return std::abs(a - b) <= epsilon;
}

} // namespace Engine
