#include <doctest/doctest.h>

#include "Math/Constants.h"
#include "Math/Quaternion.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

#include <cmath>

using namespace Engine;

TEST_CASE("Quaternion identity does not rotate") {
    const Vector3 v{1.0f, 2.0f, 3.0f};
    const Vector3 r = Quaternion::Identity().RotateVector(v);
    CHECK(r.x == doctest::Approx(v.x));
    CHECK(r.y == doctest::Approx(v.y));
    CHECK(r.z == doctest::Approx(v.z));
}

TEST_CASE("Quaternion 90 degree rotation about Y maps +X to -Z") {
    const Quaternion q = Quaternion::FromAxisAngle(Vector3::UnitY(), DegToRad(90.0f));
    const Vector3 r = q.RotateVector(Vector3::UnitX());
    CHECK(r.x == doctest::Approx(0.0f).epsilon(0.001));
    CHECK(r.y == doctest::Approx(0.0f).epsilon(0.001));
    CHECK(r.z == doctest::Approx(-1.0f).epsilon(0.001));
}

TEST_CASE("Quaternion RotateVector matches ToMatrix4 rotation") {
    const Quaternion q = Quaternion::FromAxisAngle(Vector3{1.0f, 1.0f, 0.0f}.Normalized(), DegToRad(53.0f));
    const Vector3 v{0.3f, -1.2f, 2.4f};

    const Vector3 viaQuat = q.RotateVector(v);
    const Vector4 viaMatrix4 = q.ToMatrix4() * Vector4{v, 0.0f};

    CHECK(viaQuat.x == doctest::Approx(viaMatrix4.x).epsilon(0.001));
    CHECK(viaQuat.y == doctest::Approx(viaMatrix4.y).epsilon(0.001));
    CHECK(viaQuat.z == doctest::Approx(viaMatrix4.z).epsilon(0.001));
}

TEST_CASE("Quaternion composition applies the right-hand operand first") {
    const Quaternion rotY90 = Quaternion::FromAxisAngle(Vector3::UnitY(), DegToRad(90.0f));
    const Quaternion rotX90 = Quaternion::FromAxisAngle(Vector3::UnitX(), DegToRad(90.0f));

    const Vector3 direct = rotY90.RotateVector(rotX90.RotateVector(Vector3::UnitZ()));
    const Vector3 composed = (rotY90 * rotX90).RotateVector(Vector3::UnitZ());

    CHECK(direct.x == doctest::Approx(composed.x).epsilon(0.001));
    CHECK(direct.y == doctest::Approx(composed.y).epsilon(0.001));
    CHECK(direct.z == doctest::Approx(composed.z).epsilon(0.001));
}

TEST_CASE("Quaternion Slerp endpoints match inputs") {
    const Quaternion a = Quaternion::Identity();
    const Quaternion b = Quaternion::FromAxisAngle(Vector3::UnitZ(), DegToRad(90.0f));

    const Quaternion atStart = Quaternion::Slerp(a, b, 0.0f);
    const Quaternion atEnd = Quaternion::Slerp(a, b, 1.0f);

    CHECK(atStart.w == doctest::Approx(a.w).epsilon(0.001));
    CHECK(atEnd.w == doctest::Approx(b.w).epsilon(0.001));
}

TEST_CASE("Quaternion Slerp halfway rotates half the angle") {
    const Quaternion a = Quaternion::Identity();
    const Quaternion b = Quaternion::FromAxisAngle(Vector3::UnitZ(), DegToRad(90.0f));
    const Quaternion half = Quaternion::Slerp(a, b, 0.5f);

    const Vector3 r = half.RotateVector(Vector3::UnitX());
    // Halfway to a 90-degree Z rotation of +X should land near 45 degrees.
    CHECK(r.x == doctest::Approx(std::cos(DegToRad(45.0f))).epsilon(0.01));
    CHECK(r.y == doctest::Approx(std::sin(DegToRad(45.0f))).epsilon(0.01));
}
