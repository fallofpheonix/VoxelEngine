#pragma once

#include "Math/Matrix4.h"
#include "Math/Quaternion.h"
#include "Math/Vector3.h"

namespace Engine {

struct Transform {
    Vector3 position = Vector3::Zero();
    Quaternion rotation = Quaternion::Identity();
    Vector3 scale = Vector3::One();

    Matrix4 ToMatrix4() const {
        return Matrix4::Translation(position) * rotation.ToMatrix4() * Matrix4::Scale(scale);
    }
};

} // namespace Engine
