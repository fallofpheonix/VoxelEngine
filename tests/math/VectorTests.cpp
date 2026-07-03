#include <doctest/doctest.h>

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

using namespace Engine;

TEST_CASE("Vector3 arithmetic") {
    const Vector3 a{1.0f, 2.0f, 3.0f};
    const Vector3 b{4.0f, 5.0f, 6.0f};

    CHECK((a + b).NearlyEqual({5.0f, 7.0f, 9.0f}));
    CHECK((b - a).NearlyEqual({3.0f, 3.0f, 3.0f}));
    CHECK((a * 2.0f).NearlyEqual({2.0f, 4.0f, 6.0f}));
    CHECK((-a).NearlyEqual({-1.0f, -2.0f, -3.0f}));
}

TEST_CASE("Vector3 dot and cross") {
    const Vector3 x = Vector3::UnitX();
    const Vector3 y = Vector3::UnitY();
    const Vector3 z = Vector3::UnitZ();

    CHECK(x.Dot(y) == doctest::Approx(0.0f));
    CHECK(x.Dot(x) == doctest::Approx(1.0f));
    CHECK(x.Cross(y).NearlyEqual(z)); // right-handed
}

TEST_CASE("Vector3 length and normalization") {
    const Vector3 v{3.0f, 4.0f, 0.0f};
    CHECK(v.Length() == doctest::Approx(5.0f));

    const Vector3 n = v.Normalized();
    CHECK(n.Length() == doctest::Approx(1.0f));
}

TEST_CASE("Vector3 lerp") {
    const Vector3 a{0.0f, 0.0f, 0.0f};
    const Vector3 b{10.0f, 10.0f, 10.0f};
    CHECK(Lerp(a, b, 0.5f).NearlyEqual({5.0f, 5.0f, 5.0f}));
    CHECK(Lerp(a, b, 0.0f).NearlyEqual(a));
    CHECK(Lerp(a, b, 1.0f).NearlyEqual(b));
}

TEST_CASE("Vector3 fixed world-space directions are unit length and orthogonal") {
    CHECK(Vector3::Right().NearlyEqual(-Vector3::Left()));
    CHECK(Vector3::Up().NearlyEqual(-Vector3::Down()));
    CHECK(Vector3::Forward().NearlyEqual(-Vector3::Back()));
    // Right-handed: Right x Up = Back.
    CHECK(Vector3::Right().Cross(Vector3::Up()).NearlyEqual(Vector3::Back()));
}

TEST_CASE("Vector2 basics") {
    const Vector2 a{3.0f, 4.0f};
    CHECK(a.Length() == doctest::Approx(5.0f));
    CHECK(a.Normalized().Length() == doctest::Approx(1.0f));
    CHECK((a + Vector2{1.0f, 1.0f}).NearlyEqual({4.0f, 5.0f}));
}

TEST_CASE("Vector4 basics") {
    const Vector4 a{1.0f, 2.0f, 3.0f, 1.0f};
    CHECK(a.XYZ().NearlyEqual({1.0f, 2.0f, 3.0f}));
    CHECK((a + Vector4{1.0f, 1.0f, 1.0f, 0.0f}).NearlyEqual({2.0f, 3.0f, 4.0f, 1.0f}));
}
