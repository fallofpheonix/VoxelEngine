#pragma once

#include "Math/Vector3.h"

namespace Engine {

struct Sphere {
    Vector3 center;
    float radius = 0.0f;

    Sphere() = default;
    Sphere(const Vector3& center_, float radius_) : center(center_), radius(radius_) {}

    bool Contains(const Vector3& point) const { return (point - center).LengthSquared() <= radius * radius; }

    bool Intersects(const Sphere& other) const {
        const float r = radius + other.radius;
        return (other.center - center).LengthSquared() <= r * r;
    }
};

} // namespace Engine
