#include "Foundation/Log.h"
#include "Platform/Input.h"
#include "Platform/Time.h"
#include "Platform/Window.h"

#include <GL/glew.h>

#include <cmath>
#include <memory>

// Phase 1 deliverable: a window running at a stable FPS, driven entirely
// through the Window/Input/Time platform abstractions (no direct GLFW calls
// here). glClear below is a placeholder visual signal only -- the real
// Renderer abstraction arrives in Phase 3.
int main() {
    std::unique_ptr<Engine::Window> window =
        Engine::Window::Create({"Voxel Engine - Sandbox", 1280, 720});
    Engine::Input::Init(window.get());

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
        glClearColor(0.5f + 0.5f * std::sin(t), 0.5f + 0.5f * std::sin(t + 2.0f),
                     0.5f + 0.5f * std::sin(t + 4.0f), 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        window->SwapBuffers();
    }

    return 0;
}
