#include <doctest/doctest.h>

#include "Math/Constants.h"
#include "Math/Quaternion.h"
#include "Math/Transform.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

using namespace Engine;

TEST_CASE("Transform identity maps points unchanged") {
    const Transform t;
    const Vector4 p{2.0f, 3.0f, 4.0f, 1.0f};
    const Vector4 r = t.ToMatrix4() * p;
    CHECK(r.x == doctest::Approx(p.x));
    CHECK(r.y == doctest::Approx(p.y));
    CHECK(r.z == doctest::Approx(p.z));
}

TEST_CASE("Transform composes translation, rotation, and scale") {
    Transform t;
    t.position = {10.0f, 0.0f, 0.0f};
    t.rotation = Quaternion::FromAxisAngle(Vector3::UnitY(), DegToRad(90.0f));
    t.scale = {2.0f, 2.0f, 2.0f};

    // Local +X point: scaled to 2, rotated -X->-Z (90 deg about Y maps +X to -Z),
    // then translated by +10 on X.
    const Vector4 local{1.0f, 0.0f, 0.0f, 1.0f};
    const Vector4 world = t.ToMatrix4() * local;

    CHECK(world.x == doctest::Approx(10.0f).epsilon(0.001));
    CHECK(world.y == doctest::Approx(0.0f).epsilon(0.001));
    CHECK(world.z == doctest::Approx(-2.0f).epsilon(0.001));
}
