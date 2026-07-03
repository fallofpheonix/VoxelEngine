#include <doctest/doctest.h>

#include "Math/Constants.h"
#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

#include <cmath>

using namespace Engine;

TEST_CASE("Matrix4 identity leaves vectors unchanged") {
    const Vector4 v{1.0f, 2.0f, 3.0f, 1.0f};
    const Vector4 r = Matrix4::Identity() * v;
    CHECK(r.x == doctest::Approx(v.x));
    CHECK(r.y == doctest::Approx(v.y));
    CHECK(r.z == doctest::Approx(v.z));
    CHECK(r.w == doctest::Approx(v.w));
}

TEST_CASE("Matrix4 translation moves a point") {
    const Matrix4 t = Matrix4::Translation({10.0f, -5.0f, 2.0f});
    const Vector4 p{0.0f, 0.0f, 0.0f, 1.0f}; // point (w=1)
    const Vector4 r = t * p;
    CHECK(r.x == doctest::Approx(10.0f));
    CHECK(r.y == doctest::Approx(-5.0f));
    CHECK(r.z == doctest::Approx(2.0f));

    // A direction (w=0) is unaffected by translation.
    const Vector4 d{1.0f, 0.0f, 0.0f, 0.0f};
    const Vector4 rd = t * d;
    CHECK(rd.x == doctest::Approx(1.0f));
    CHECK(rd.y == doctest::Approx(0.0f));
}

TEST_CASE("Matrix4 scale scales a point") {
    const Matrix4 s = Matrix4::Scale({2.0f, 3.0f, 4.0f});
    const Vector4 r = s * Vector4{1.0f, 1.0f, 1.0f, 1.0f};
    CHECK(r.x == doctest::Approx(2.0f));
    CHECK(r.y == doctest::Approx(3.0f));
    CHECK(r.z == doctest::Approx(4.0f));
}

TEST_CASE("Matrix4 rotation about Z maps +X to +Y at 90 degrees") {
    const Matrix4 r = Matrix4::RotationZ(DegToRad(90.0f));
    const Vector4 result = r * Vector4{1.0f, 0.0f, 0.0f, 0.0f};
    CHECK(result.x == doctest::Approx(0.0f).epsilon(0.001));
    CHECK(result.y == doctest::Approx(1.0f).epsilon(0.001));
}

TEST_CASE("Matrix4 multiplication composes transforms left-to-right on the vector") {
    // translate(scale(v)) != scale(translate(v)) in general -- verify order.
    const Matrix4 t = Matrix4::Translation({1.0f, 0.0f, 0.0f});
    const Matrix4 s = Matrix4::Scale({2.0f, 2.0f, 2.0f});
    const Vector4 v{1.0f, 0.0f, 0.0f, 1.0f};

    const Vector4 scaleThenTranslate = (t * s) * v; // scale first, then translate
    CHECK(scaleThenTranslate.x == doctest::Approx(3.0f)); // (1*2) + 1

    const Vector4 translateThenScale = (s * t) * v; // translate first, then scale
    CHECK(translateThenScale.x == doctest::Approx(4.0f)); // (1+1) * 2
}

TEST_CASE("Matrix4 transpose is its own inverse operation") {
    const Matrix4 m = Matrix4::Translation({1.0f, 2.0f, 3.0f});
    CHECK(m.Transpose().Transpose().NearlyEqual(m));
}

TEST_CASE("Matrix4 inverse of translation undoes the translation") {
    const Matrix4 t = Matrix4::Translation({5.0f, -3.0f, 7.0f});
    const Matrix4 inv = t.Inverse();
    CHECK((t * inv).NearlyEqual(Matrix4::Identity()));
}

TEST_CASE("Matrix4 inverse of a composed TRS round-trips a point") {
    const Matrix4 m = Matrix4::Translation({3.0f, 1.0f, -2.0f}) *
                       Matrix4::RotationY(DegToRad(37.0f)) *
                       Matrix4::Scale({2.0f, 0.5f, 1.5f});
    const Matrix4 inv = m.Inverse();

    const Vector4 p{1.0f, 2.0f, 3.0f, 1.0f};
    const Vector4 roundTripped = inv * (m * p);

    CHECK(roundTripped.x == doctest::Approx(p.x).epsilon(0.001));
    CHECK(roundTripped.y == doctest::Approx(p.y).epsilon(0.001));
    CHECK(roundTripped.z == doctest::Approx(p.z).epsilon(0.001));
}

TEST_CASE("Matrix4 perspective projects a point in front of the camera to positive w") {
    const Matrix4 proj = Matrix4::Perspective(DegToRad(60.0f), 16.0f / 9.0f, 0.1f, 100.0f);
    const Vector4 clip = proj * Vector4{0.0f, 0.0f, -5.0f, 1.0f};
    CHECK(clip.w > 0.0f);
    // NDC z should land inside [-1, 1] for a point between near and far.
    const float ndcZ = clip.z / clip.w;
    CHECK(ndcZ >= -1.0f);
    CHECK(ndcZ <= 1.0f);
}

TEST_CASE("Matrix4 LookAt places the eye at the origin of view space") {
    const Matrix4 view = Matrix4::LookAt({0.0f, 0.0f, 5.0f}, {0.0f, 0.0f, 0.0f}, Vector3::UnitY());
    const Vector4 eyeInView = view * Vector4{0.0f, 0.0f, 5.0f, 1.0f};
    CHECK(eyeInView.x == doctest::Approx(0.0f).epsilon(0.001));
    CHECK(eyeInView.y == doctest::Approx(0.0f).epsilon(0.001));
    CHECK(eyeInView.z == doctest::Approx(0.0f).epsilon(0.001));
}

TEST_CASE("Matrix3 normal matrix leaves normals correct under non-uniform scale") {
    // A normal perpendicular to the scaled axis should stay perpendicular
    // to the scaled surface -- this is exactly what a plain (non-inverse-
    // transpose) matrix gets wrong.
    const Matrix4 model = Matrix4::Scale({1.0f, 1.0f, 2.0f}); // stretch along Z
    const Matrix3 normalMatrix = Matrix3::NormalMatrix(model);

    const Vector3 normal = Vector3::UnitZ(); // normal of the XY face
    const Vector3 transformed = (normalMatrix * normal).Normalized();
    CHECK(transformed.x == doctest::Approx(0.0f).epsilon(0.001));
    CHECK(transformed.y == doctest::Approx(0.0f).epsilon(0.001));
    CHECK(std::abs(transformed.z) == doctest::Approx(1.0f).epsilon(0.001));
}
