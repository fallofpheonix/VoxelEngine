#pragma once

#include "Math/Vector3.h"

#include <algorithm>
#include <limits>

namespace Engine {

struct AABB {
    Vector3 min{std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
    Vector3 max{std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()};

    AABB() = default;
    AABB(const Vector3& min_, const Vector3& max_) : min(min_), max(max_) {}

    Vector3 Center() const { return (min + max) * 0.5f; }
    Vector3 Extents() const { return (max - min) * 0.5f; }

    bool Contains(const Vector3& point) const {
        return point.x >= min.x && point.x <= max.x &&
               point.y >= min.y && point.y <= max.y &&
               point.z >= min.z && point.z <= max.z;
    }

    bool Intersects(const AABB& other) const {
        return min.x <= other.max.x && max.x >= other.min.x &&
               min.y <= other.max.y && max.y >= other.min.y &&
               min.z <= other.max.z && max.z >= other.min.z;
    }

    AABB Union(const AABB& other) const {
        return {
            {std::min(min.x, other.min.x), std::min(min.y, other.min.y), std::min(min.z, other.min.z)},
            {std::max(max.x, other.max.x), std::max(max.y, other.max.y), std::max(max.z, other.max.z)},
        };
    }
};

} // namespace Engine
