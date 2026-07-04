#include "Foundation/Log.h"
#include "Math/Math.h"
#include "Platform/Input.h"
#include "Platform/Time.h"
#include "Platform/Window.h"
#include "Renderer/IRenderer.h"

#include <cmath>
#include <memory>
#include <string>
#include <vector>

static const char* kVertexShader = R"(
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vNormal;
out vec2 vUV;

void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
    vNormal = mat3(uModel) * aNormal;
    vUV = aUV;
}
)";

static const char* kFragmentShader = R"(
#version 330 core
in vec3 vNormal;
in vec2 vUV;

uniform vec3 uColor;

out vec4 FragColor;

void main() {
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float diff = max(dot(normalize(vNormal), lightDir), 0.15);
    FragColor = vec4(uColor * diff, 1.0);
}
)";

// Step 10: hardcoded triangle
static Engine::MeshData MakeTriangle() {
    Engine::MeshData data;
    data.vertices = {
        {{ 0.0f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 1.0f}},
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
    };
    data.indices = {0, 1, 2};
    return data;
}

// Step 11: cube (24 vertices for proper normals per face)
static Engine::MeshData MakeCube() {
    Engine::MeshData data;
    data.vertices = {
        // front face (+Z)
        {{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},
        // back face (-Z)
        {{ 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},
        {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},
        // top face (+Y)
        {{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}},
        // bottom face (-Y)
        {{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
        {{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}},
        // right face (+X)
        {{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
        {{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
        // left face (-X)
        {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
    };
    data.indices = {
         0,  1,  2,   2,  3,  0,
         4,  5,  6,   6,  7,  4,
         8,  9, 10,  10, 11,  8,
        12, 13, 14,  14, 15, 12,
        16, 17, 18,  18, 19, 16,
        20, 21, 22,  22, 23, 20,
    };
    return data;
}

// Step 12: procedural grid mesh (non-trivial vertex count)
static Engine::MeshData MakeGrid(int size) {
    Engine::MeshData data;
    const float half = static_cast<float>(size) * 0.5f;

    for (int z = 0; z <= size; ++z) {
        for (int x = 0; x <= size; ++x) {
            float fx = static_cast<float>(x) - half;
            float fz = static_cast<float>(z) - half;
            float u = static_cast<float>(x) / static_cast<float>(size);
            float v = static_cast<float>(z) / static_cast<float>(size);
            data.vertices.push_back({{fx, 0.0f, fz}, {0.0f, 1.0f, 0.0f}, {u, v}});
        }
    }

    for (int z = 0; z < size; ++z) {
        for (int x = 0; x < size; ++x) {
            uint32_t tl = static_cast<uint32_t>(z * (size + 1) + x);
            uint32_t tr = tl + 1;
            uint32_t bl = tl + static_cast<uint32_t>(size + 1);
            uint32_t br = bl + 1;
            data.indices.insert(data.indices.end(), {tl, bl, tr, tr, bl, br});
        }
    }
    return data;
}

int main() {
    std::unique_ptr<Engine::Window> window =
        Engine::Window::Create({"Voxel Engine - Sandbox", 1280, 720});
    Engine::Input::Init(window.get());

    std::unique_ptr<Engine::IRenderer> renderer = Engine::IRenderer::Create(window.get());

    // Step 2: compile shaders
    Engine::ShaderHandle shader = renderer->CreateShader(kVertexShader, kFragmentShader);
    if (!shader.IsValid()) {
        ENGINE_LOG_ERROR("Failed to compile shaders, exiting");
        return 1;
    }
    ENGINE_LOG_INFO("Shaders compiled successfully");

    // Step 10: triangle
    Engine::MeshHandle triangleMesh = renderer->CreateMesh(MakeTriangle());

    // Step 11: cube
    Engine::MeshHandle cubeMesh = renderer->CreateMesh(MakeCube());

    // Step 12: grid (10x10 = 121 vertices, 200 triangles)
    Engine::MeshHandle gridMesh = renderer->CreateMesh(MakeGrid(10));
    ENGINE_LOG_INFO("Grid mesh: 121 vertices, 200 triangles");

    // Step 13: scene -- multiple objects with different colors
    struct SceneObject {
        Engine::MeshHandle mesh;
        Engine::Vector3 position;
        Engine::Vector3 color;
        float rotationSpeed;
    };

    std::vector<SceneObject> scene = {
        {triangleMesh, {-2.5f, 0.0f, 0.0f}, {0.9f, 0.2f, 0.2f}, 0.7f},
        {cubeMesh,     { 0.0f, 0.5f, 0.0f}, {0.2f, 0.7f, 0.3f}, 1.0f},
        {gridMesh,     { 2.5f, 0.0f, 0.0f}, {0.2f, 0.3f, 0.9f}, 0.0f},
    };

    const float aspect = 1280.0f / 720.0f;
    const Engine::Vector3 eye{0.0f, 2.0f, 5.0f};
    const Engine::Vector3 target{0.0f, 0.0f, 0.0f};

    float fpsAccumulator = 0.0f;
    int frameCount = 0;

    while (!window->ShouldClose()) {
        window->PollEvents();
        Engine::Time::Update();

        if (Engine::Input::IsKeyPressed(Engine::Key::Escape)) {
            break;
        }

        fpsAccumulator += Engine::Time::DeltaTime();
        ++frameCount;
        if (fpsAccumulator >= 1.0f) {
            ENGINE_LOG_INFO("FPS: " + std::to_string(frameCount));
            frameCount = 0;
            fpsAccumulator = 0.0f;
        }

        const float t = Engine::Time::GetTime();

        Engine::Camera cam = Engine::Camera::Perspective(
            eye, target, Engine::DegToRad(45.0f), aspect, 0.1f, 100.0f);

        renderer->BeginFrame(cam);

        // Step 13: submit multiple objects with different transforms/colors
        for (const auto& obj : scene) {
            Engine::Matrix4 model = Engine::Matrix4::Translation(obj.position);
            if (obj.rotationSpeed != 0.0f) {
                model = model * Engine::Matrix4::RotationY(t * obj.rotationSpeed);
            }
            Engine::RenderCommand cmd;
            cmd.mesh = obj.mesh;
            cmd.material.shader = shader;
            cmd.material.color = obj.color;
            cmd.modelMatrix = model;
            renderer->Submit(cmd);
        }

        renderer->EndFrame();
        window->SwapBuffers();
    }

    return 0;
}
