#pragma once

#include "Foundation/Assert.h"
#include "Math/Matrix4.h"
#include "Math/Vector3.h"

namespace Engine {

// Column-major storage (m[col * 3 + row]). Primarily used as a normal
// matrix (inverse-transpose of a Matrix4's upper-left 3x3).
struct Matrix3 {
    float m[9] = {
        1, 0, 0,
        0, 1, 0,
        0, 0, 1,
    };

    Matrix3() = default;

    float& At(int row, int col) { return m[col * 3 + row]; }
    float At(int row, int col) const { return m[col * 3 + row]; }

    static Matrix3 Identity() { return Matrix3(); }

    static Matrix3 FromUpperLeft(const Matrix4& mat4) {
        Matrix3 r;
        for (int col = 0; col < 3; ++col) {
            for (int row = 0; row < 3; ++row) {
                r.At(row, col) = mat4.At(row, col);
            }
        }
        return r;
    }

    Vector3 operator*(const Vector3& v) const {
        return {
            At(0, 0) * v.x + At(0, 1) * v.y + At(0, 2) * v.z,
            At(1, 0) * v.x + At(1, 1) * v.y + At(1, 2) * v.z,
            At(2, 0) * v.x + At(2, 1) * v.y + At(2, 2) * v.z,
        };
    }

    Matrix3 Transpose() const {
        Matrix3 r;
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                r.At(col, row) = At(row, col);
            }
        }
        return r;
    }

    float Determinant() const {
        return At(0, 0) * (At(1, 1) * At(2, 2) - At(1, 2) * At(2, 1)) -
               At(0, 1) * (At(1, 0) * At(2, 2) - At(1, 2) * At(2, 0)) +
               At(0, 2) * (At(1, 0) * At(2, 1) - At(1, 1) * At(2, 0));
    }

    Matrix3 Inverse() const {
        const float det = Determinant();
        ENGINE_ASSERT(det != 0.0f, "Matrix3::Inverse: matrix is singular");
        const float invDet = 1.0f / det;

        Matrix3 r;
        r.At(0, 0) = (At(1, 1) * At(2, 2) - At(1, 2) * At(2, 1)) * invDet;
        r.At(0, 1) = (At(0, 2) * At(2, 1) - At(0, 1) * At(2, 2)) * invDet;
        r.At(0, 2) = (At(0, 1) * At(1, 2) - At(0, 2) * At(1, 1)) * invDet;
        r.At(1, 0) = (At(1, 2) * At(2, 0) - At(1, 0) * At(2, 2)) * invDet;
        r.At(1, 1) = (At(0, 0) * At(2, 2) - At(0, 2) * At(2, 0)) * invDet;
        r.At(1, 2) = (At(0, 2) * At(1, 0) - At(0, 0) * At(1, 2)) * invDet;
        r.At(2, 0) = (At(1, 0) * At(2, 1) - At(1, 1) * At(2, 0)) * invDet;
        r.At(2, 1) = (At(0, 1) * At(2, 0) - At(0, 0) * At(2, 1)) * invDet;
        r.At(2, 2) = (At(0, 0) * At(1, 1) - At(0, 1) * At(1, 0)) * invDet;
        return r;
    }

    // The normal matrix is the inverse-transpose of the model matrix's
    // upper-left 3x3, so non-uniform scale doesn't skew normals.
    static Matrix3 NormalMatrix(const Matrix4& modelMatrix) {
        return FromUpperLeft(modelMatrix).Inverse().Transpose();
    }
};

} // namespace Engine
