#pragma once

#include "Math/Vector3.h"

namespace Engine {

// Represents { p : dot(normal, p) = distance }.
struct Plane {
    Vector3 normal = Vector3::UnitY();
    float distance = 0.0f;

    Plane() = default;
    Plane(const Vector3& normal_, float distance_) : normal(normal_), distance(distance_) {}

    static Plane FromNormalAndPoint(const Vector3& normal_, const Vector3& point) {
        const Vector3 n = normal_.Normalized();
        return {n, n.Dot(point)};
    }

    float DistanceToPoint(const Vector3& point) const { return normal.Dot(point) - distance; }
};

} // namespace Engine
