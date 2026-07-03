#include <doctest/doctest.h>

#include "Math/AABB.h"
#include "Math/Intersections.h"
#include "Math/Plane.h"
#include "Math/Ray.h"
#include "Math/Sphere.h"
#include "Math/Vector3.h"

using namespace Engine;

TEST_CASE("AABB contains and intersects") {
    const AABB box{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};
    CHECK(box.Contains({0.0f, 0.0f, 0.0f}));
    CHECK_FALSE(box.Contains({2.0f, 0.0f, 0.0f}));

    const AABB overlapping{{0.5f, 0.5f, 0.5f}, {2.0f, 2.0f, 2.0f}};
    CHECK(box.Intersects(overlapping));

    const AABB disjoint{{5.0f, 5.0f, 5.0f}, {6.0f, 6.0f, 6.0f}};
    CHECK_FALSE(box.Intersects(disjoint));
}

TEST_CASE("AABB union covers both boxes") {
    const AABB a{{-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 1.0f}};
    const AABB b{{0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 1.0f}};
    const AABB u = a.Union(b);
    CHECK(u.min.x == doctest::Approx(-1.0f));
    CHECK(u.min.y == doctest::Approx(-1.0f));
    CHECK(u.max.x == doctest::Approx(1.0f));
    CHECK(u.max.y == doctest::Approx(1.0f));
}

TEST_CASE("Sphere contains and intersects") {
    const Sphere s{{0.0f, 0.0f, 0.0f}, 2.0f};
    CHECK(s.Contains({1.0f, 1.0f, 0.0f}));
    CHECK_FALSE(s.Contains({5.0f, 0.0f, 0.0f}));

    const Sphere touching{{3.0f, 0.0f, 0.0f}, 1.5f};
    CHECK(s.Intersects(touching));

    const Sphere far{{10.0f, 0.0f, 0.0f}, 1.0f};
    CHECK_FALSE(s.Intersects(far));
}

TEST_CASE("Plane distance to point") {
    const Plane groundPlane = Plane::FromNormalAndPoint(Vector3::UnitY(), {0.0f, 0.0f, 0.0f});
    CHECK(groundPlane.DistanceToPoint({0.0f, 5.0f, 0.0f}) == doctest::Approx(5.0f));
    CHECK(groundPlane.DistanceToPoint({0.0f, -3.0f, 0.0f}) == doctest::Approx(-3.0f));
}

TEST_CASE("Ray-AABB intersection: hit from outside") {
    const AABB box{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};
    const Ray ray{{-5.0f, 0.0f, 0.0f}, Vector3::UnitX()};
    float t = -1.0f;
    CHECK(RayIntersectsAABB(ray, box, t));
    CHECK(t == doctest::Approx(4.0f));
}

TEST_CASE("Ray-AABB intersection: miss") {
    const AABB box{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};
    const Ray ray{{-5.0f, 5.0f, 0.0f}, Vector3::UnitX()};
    float t = -1.0f;
    CHECK_FALSE(RayIntersectsAABB(ray, box, t));
}

TEST_CASE("Ray-Sphere intersection: hit and miss") {
    const Sphere sphere{{0.0f, 0.0f, 0.0f}, 1.0f};

    const Ray hitRay{{-5.0f, 0.0f, 0.0f}, Vector3::UnitX()};
    float t = -1.0f;
    CHECK(RayIntersectsSphere(hitRay, sphere, t));
    CHECK(t == doctest::Approx(4.0f));

    const Ray missRay{{-5.0f, 5.0f, 0.0f}, Vector3::UnitX()};
    CHECK_FALSE(RayIntersectsSphere(missRay, sphere, t));
}
