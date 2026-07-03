#pragma once

#include "Math/Vector3.h"

namespace Engine {

struct Ray {
    Vector3 origin;
    Vector3 direction; // expected normalized; callers own that invariant

    Ray() = default;
    Ray(const Vector3& origin_, const Vector3& direction_) : origin(origin_), direction(direction_) {}

    Vector3 PointAt(float t) const { return origin + direction * t; }
};

} // namespace Engine
