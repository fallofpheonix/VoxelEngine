#pragma once

#include "Math/Matrix4.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Renderer/Handles.h"

#include <cstdint>
#include <vector>

namespace Engine {

struct Vertex {
    Vector3 position;
    Vector3 normal;
    Vector2 uv;
};

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

struct Material {
    ShaderHandle shader;
    TextureHandle texture;
};

struct Camera {
    Matrix4 view;
    Matrix4 projection;

    static Camera Perspective(const Vector3& eye, const Vector3& target, float fovYRadians,
                              float aspect, float nearZ, float farZ);
};

struct RenderCommand {
    MeshHandle mesh;
    Material material;
    Matrix4 modelMatrix;
};

} // namespace Engine
