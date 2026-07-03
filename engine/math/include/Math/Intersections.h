#pragma once

#include "Math/AABB.h"
#include "Math/Ray.h"
#include "Math/Sphere.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace Engine {

// Slab method. Assumes ray.direction need not be normalized. On hit, tOut is
// the distance along the ray to the nearest intersection (clamped to >= 0).
inline bool RayIntersectsAABB(const Ray& ray, const AABB& box, float& tOut) {
    float tMin = 0.0f;
    float tMax = std::numeric_limits<float>::max();

    const float origin[3] = {ray.origin.x, ray.origin.y, ray.origin.z};
    const float dir[3] = {ray.direction.x, ray.direction.y, ray.direction.z};
    const float bmin[3] = {box.min.x, box.min.y, box.min.z};
    const float bmax[3] = {box.max.x, box.max.y, box.max.z};

    for (int i = 0; i < 3; ++i) {
        if (std::abs(dir[i]) < 1e-8f) {
            if (origin[i] < bmin[i] || origin[i] > bmax[i]) {
                return false;
            }
        } else {
            const float invD = 1.0f / dir[i];
            float t1 = (bmin[i] - origin[i]) * invD;
            float t2 = (bmax[i] - origin[i]) * invD;
            if (t1 > t2) std::swap(t1, t2);
            tMin = std::max(tMin, t1);
            tMax = std::min(tMax, t2);
            if (tMin > tMax) {
                return false;
            }
        }
    }
    tOut = tMin;
    return true;
}

// Assumes ray.direction is normalized. On hit, tOut is the distance along
// the ray to the nearest intersection in front of the origin.
inline bool RayIntersectsSphere(const Ray& ray, const Sphere& sphere, float& tOut) {
    const Vector3 oc = ray.origin - sphere.center;
    const float b = oc.Dot(ray.direction);
    const float c = oc.LengthSquared() - sphere.radius * sphere.radius;
    const float discriminant = b * b - c;
    if (discriminant < 0.0f) {
        return false;
    }
    const float sqrtD = std::sqrt(discriminant);
    const float t0 = -b - sqrtD;
    const float t1 = -b + sqrtD;
    if (t0 < 0.0f) {
        if (t1 < 0.0f) {
            return false;
        }
        tOut = t1;
    } else {
        tOut = t0;
    }
    return true;
}

} // namespace Engine
