#pragma once

#include "Foundation/Assert.h"
#include "Math/Constants.h"
#include "Math/Matrix4.h"
#include "Math/Vector3.h"

#include <cmath>

namespace Engine {

struct Quaternion {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;

    Quaternion() = default;
    Quaternion(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}

    static Quaternion Identity() { return {0.0f, 0.0f, 0.0f, 1.0f}; }

    static Quaternion FromAxisAngle(const Vector3& axis, float radians) {
        const Vector3 n = axis.Normalized();
        const float half = radians * 0.5f;
        const float s = std::sin(half);
        return {n.x * s, n.y * s, n.z * s, std::cos(half)};
    }

    float LengthSquared() const { return x * x + y * y + z * z + w * w; }
    float Length() const { return std::sqrt(LengthSquared()); }

    Quaternion Normalized() const {
        const float len = Length();
        ENGINE_ASSERT(len > 0.0f, "Cannot normalize a zero-length Quaternion");
        return {x / len, y / len, z / len, w / len};
    }

    Quaternion Conjugate() const { return {-x, -y, -z, w}; }

    // Hamilton product: applying (a * b) rotates by b first, then a.
    Quaternion operator*(const Quaternion& b) const {
        return {
            w * b.x + x * b.w + y * b.z - z * b.y,
            w * b.y - x * b.z + y * b.w + z * b.x,
            w * b.z + x * b.y - y * b.x + z * b.w,
            w * b.w - x * b.x - y * b.y - z * b.z,
        };
    }

    Vector3 RotateVector(const Vector3& v) const {
        const Quaternion vq{v.x, v.y, v.z, 0.0f};
        const Quaternion r = (*this) * vq * Conjugate();
        return {r.x, r.y, r.z};
    }

    Matrix4 ToMatrix4() const {
        Matrix4 m;
        const float xx = x * x, yy = y * y, zz = z * z;
        const float xy = x * y, xz = x * z, yz = y * z;
        const float wx = w * x, wy = w * y, wz = w * z;

        m.At(0, 0) = 1.0f - 2.0f * (yy + zz);
        m.At(0, 1) = 2.0f * (xy - wz);
        m.At(0, 2) = 2.0f * (xz + wy);

        m.At(1, 0) = 2.0f * (xy + wz);
        m.At(1, 1) = 1.0f - 2.0f * (xx + zz);
        m.At(1, 2) = 2.0f * (yz - wx);

        m.At(2, 0) = 2.0f * (xz - wy);
        m.At(2, 1) = 2.0f * (yz + wx);
        m.At(2, 2) = 1.0f - 2.0f * (xx + yy);

        return m;
    }

    static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t) {
        Quaternion end = b;
        float cosHalfTheta = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;

        // Take the shorter path.
        if (cosHalfTheta < 0.0f) {
            end = {-b.x, -b.y, -b.z, -b.w};
            cosHalfTheta = -cosHalfTheta;
        }

        if (cosHalfTheta > 0.9995f) {
            // Nearly identical rotations: linear interpolation avoids a
            // division-by-near-zero further down.
            return Quaternion{
                Lerp(a.x, end.x, t), Lerp(a.y, end.y, t),
                Lerp(a.z, end.z, t), Lerp(a.w, end.w, t),
            }.Normalized();
        }

        const float halfTheta = std::acos(cosHalfTheta);
        const float sinHalfTheta = std::sqrt(1.0f - cosHalfTheta * cosHalfTheta);

        const float ratioA = std::sin((1.0f - t) * halfTheta) / sinHalfTheta;
        const float ratioB = std::sin(t * halfTheta) / sinHalfTheta;

        return {
            a.x * ratioA + end.x * ratioB,
            a.y * ratioA + end.y * ratioB,
            a.z * ratioA + end.z * ratioB,
            a.w * ratioA + end.w * ratioB,
        };
    }
};

} // namespace Engine
