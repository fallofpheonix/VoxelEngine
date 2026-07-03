#pragma once

#include "Foundation/Assert.h"
#include "Math/Constants.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

#include <cmath>

namespace Engine {

// Column-major storage (m[col * 4 + row]), matching OpenGL's convention so
// Data() can be passed directly to glUniformMatrix4fv with transpose=GL_FALSE.
struct Matrix4 {
    float m[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };

    Matrix4() = default;

    float& At(int row, int col) { return m[col * 4 + row]; }
    float At(int row, int col) const { return m[col * 4 + row]; }

    float* Data() { return m; }
    const float* Data() const { return m; }

    static Matrix4 Identity() { return Matrix4(); }

    static Matrix4 Translation(const Vector3& t) {
        Matrix4 r;
        r.At(0, 3) = t.x;
        r.At(1, 3) = t.y;
        r.At(2, 3) = t.z;
        return r;
    }

    static Matrix4 Scale(const Vector3& s) {
        Matrix4 r;
        r.At(0, 0) = s.x;
        r.At(1, 1) = s.y;
        r.At(2, 2) = s.z;
        return r;
    }

    static Matrix4 RotationX(float radians) {
        Matrix4 r;
        const float c = std::cos(radians);
        const float s = std::sin(radians);
        r.At(1, 1) = c;  r.At(1, 2) = -s;
        r.At(2, 1) = s;  r.At(2, 2) = c;
        return r;
    }

    static Matrix4 RotationY(float radians) {
        Matrix4 r;
        const float c = std::cos(radians);
        const float s = std::sin(radians);
        r.At(0, 0) = c;   r.At(0, 2) = s;
        r.At(2, 0) = -s;  r.At(2, 2) = c;
        return r;
    }

    static Matrix4 RotationZ(float radians) {
        Matrix4 r;
        const float c = std::cos(radians);
        const float s = std::sin(radians);
        r.At(0, 0) = c;  r.At(0, 1) = -s;
        r.At(1, 0) = s;  r.At(1, 1) = c;
        return r;
    }

    // Right-handed, OpenGL NDC depth range [-1, 1].
    static Matrix4 Perspective(float fovYRadians, float aspect, float nearZ, float farZ) {
        ENGINE_ASSERT(aspect != 0.0f, "Perspective: aspect must be non-zero");
        ENGINE_ASSERT(farZ != nearZ, "Perspective: nearZ and farZ must differ");
        const float tanHalfFovY = std::tan(fovYRadians * 0.5f);
        Matrix4 r;
        for (float& v : r.m) v = 0.0f;
        r.At(0, 0) = 1.0f / (aspect * tanHalfFovY);
        r.At(1, 1) = 1.0f / tanHalfFovY;
        r.At(2, 2) = -(farZ + nearZ) / (farZ - nearZ);
        r.At(2, 3) = -(2.0f * farZ * nearZ) / (farZ - nearZ);
        r.At(3, 2) = -1.0f;
        return r;
    }

    // Right-handed, OpenGL NDC depth range [-1, 1].
    static Matrix4 Orthographic(float left, float right, float bottom, float top, float nearZ, float farZ) {
        Matrix4 r;
        r.At(0, 0) = 2.0f / (right - left);
        r.At(1, 1) = 2.0f / (top - bottom);
        r.At(2, 2) = -2.0f / (farZ - nearZ);
        r.At(0, 3) = -(right + left) / (right - left);
        r.At(1, 3) = -(top + bottom) / (top - bottom);
        r.At(2, 3) = -(farZ + nearZ) / (farZ - nearZ);
        return r;
    }

    // Right-handed.
    static Matrix4 LookAt(const Vector3& eye, const Vector3& center, const Vector3& up) {
        const Vector3 f = (center - eye).Normalized();
        const Vector3 s = f.Cross(up).Normalized();
        const Vector3 u = s.Cross(f);

        Matrix4 r;
        r.At(0, 0) = s.x;  r.At(0, 1) = s.y;  r.At(0, 2) = s.z;  r.At(0, 3) = -s.Dot(eye);
        r.At(1, 0) = u.x;  r.At(1, 1) = u.y;  r.At(1, 2) = u.z;  r.At(1, 3) = -u.Dot(eye);
        r.At(2, 0) = -f.x; r.At(2, 1) = -f.y; r.At(2, 2) = -f.z; r.At(2, 3) = f.Dot(eye);
        r.At(3, 0) = 0;    r.At(3, 1) = 0;    r.At(3, 2) = 0;    r.At(3, 3) = 1;
        return r;
    }

    Matrix4 operator*(const Matrix4& rhs) const {
        Matrix4 result;
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    sum += At(row, k) * rhs.At(k, col);
                }
                result.At(row, col) = sum;
            }
        }
        return result;
    }

    Vector4 operator*(const Vector4& v) const {
        return {
            At(0, 0) * v.x + At(0, 1) * v.y + At(0, 2) * v.z + At(0, 3) * v.w,
            At(1, 0) * v.x + At(1, 1) * v.y + At(1, 2) * v.z + At(1, 3) * v.w,
            At(2, 0) * v.x + At(2, 1) * v.y + At(2, 2) * v.z + At(2, 3) * v.w,
            At(3, 0) * v.x + At(3, 1) * v.y + At(3, 2) * v.z + At(3, 3) * v.w,
        };
    }

    // No operator==: exact float comparison is almost never correct for a
    // computed matrix. See docs/decisions/0007-floating-point-equality.md.
    bool NearlyEqual(const Matrix4& rhs, float epsilon = kEpsilon) const {
        for (int i = 0; i < 16; ++i) {
            if (std::abs(m[i] - rhs.m[i]) > epsilon) return false;
        }
        return true;
    }

    Matrix4 Transpose() const {
        Matrix4 r;
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                r.At(col, row) = At(row, col);
            }
        }
        return r;
    }

    // General 4x4 inverse via cofactor expansion. Asserts if singular.
    Matrix4 Inverse() const {
        const float* a = m;
        float inv[16];

        inv[0] = a[5] * a[10] * a[15] - a[5] * a[11] * a[14] - a[9] * a[6] * a[15] +
                 a[9] * a[7] * a[14] + a[13] * a[6] * a[11] - a[13] * a[7] * a[10];
        inv[4] = -a[4] * a[10] * a[15] + a[4] * a[11] * a[14] + a[8] * a[6] * a[15] -
                  a[8] * a[7] * a[14] - a[12] * a[6] * a[11] + a[12] * a[7] * a[10];
        inv[8] = a[4] * a[9] * a[15] - a[4] * a[11] * a[13] - a[8] * a[5] * a[15] +
                 a[8] * a[7] * a[13] + a[12] * a[5] * a[11] - a[12] * a[7] * a[9];
        inv[12] = -a[4] * a[9] * a[14] + a[4] * a[10] * a[13] + a[8] * a[5] * a[14] -
                   a[8] * a[6] * a[13] - a[12] * a[5] * a[10] + a[12] * a[6] * a[9];

        inv[1] = -a[1] * a[10] * a[15] + a[1] * a[11] * a[14] + a[9] * a[2] * a[15] -
                  a[9] * a[3] * a[14] - a[13] * a[2] * a[11] + a[13] * a[3] * a[10];
        inv[5] = a[0] * a[10] * a[15] - a[0] * a[11] * a[14] - a[8] * a[2] * a[15] +
                 a[8] * a[3] * a[14] + a[12] * a[2] * a[11] - a[12] * a[3] * a[10];
        inv[9] = -a[0] * a[9] * a[15] + a[0] * a[11] * a[13] + a[8] * a[1] * a[15] -
                  a[8] * a[3] * a[13] - a[12] * a[1] * a[11] + a[12] * a[3] * a[9];
        inv[13] = a[0] * a[9] * a[14] - a[0] * a[10] * a[13] - a[8] * a[1] * a[14] +
                  a[8] * a[2] * a[13] + a[12] * a[1] * a[10] - a[12] * a[2] * a[9];

        inv[2] = a[1] * a[6] * a[15] - a[1] * a[7] * a[14] - a[5] * a[2] * a[15] +
                 a[5] * a[3] * a[14] + a[13] * a[2] * a[7] - a[13] * a[3] * a[6];
        inv[6] = -a[0] * a[6] * a[15] + a[0] * a[7] * a[14] + a[4] * a[2] * a[15] -
                  a[4] * a[3] * a[14] - a[12] * a[2] * a[7] + a[12] * a[3] * a[6];
        inv[10] = a[0] * a[5] * a[15] - a[0] * a[7] * a[13] - a[4] * a[1] * a[15] +
                  a[4] * a[3] * a[13] + a[12] * a[1] * a[7] - a[12] * a[3] * a[5];
        inv[14] = -a[0] * a[5] * a[14] + a[0] * a[6] * a[13] + a[4] * a[1] * a[14] -
                   a[4] * a[2] * a[13] - a[12] * a[1] * a[6] + a[12] * a[2] * a[5];

        inv[3] = -a[1] * a[6] * a[11] + a[1] * a[7] * a[10] + a[5] * a[2] * a[11] -
                  a[5] * a[3] * a[10] - a[9] * a[2] * a[7] + a[9] * a[3] * a[6];
        inv[7] = a[0] * a[6] * a[11] - a[0] * a[7] * a[10] - a[4] * a[2] * a[11] +
                 a[4] * a[3] * a[10] + a[8] * a[2] * a[7] - a[8] * a[3] * a[6];
        inv[11] = -a[0] * a[5] * a[11] + a[0] * a[7] * a[9] + a[4] * a[1] * a[11] -
                   a[4] * a[3] * a[9] - a[8] * a[1] * a[7] + a[8] * a[3] * a[5];
        inv[15] = a[0] * a[5] * a[10] - a[0] * a[6] * a[9] - a[4] * a[1] * a[10] +
                  a[4] * a[2] * a[9] + a[8] * a[1] * a[6] - a[8] * a[2] * a[5];

        float det = a[0] * inv[0] + a[1] * inv[4] + a[2] * inv[8] + a[3] * inv[12];
        ENGINE_ASSERT(det != 0.0f, "Matrix4::Inverse: matrix is singular");
        det = 1.0f / det;

        Matrix4 result;
        for (int i = 0; i < 16; ++i) {
            result.m[i] = inv[i] * det;
        }
        return result;
    }
};

} // namespace Engine
