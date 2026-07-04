#include "Foundation/Log.h"
#include "Platform/Input.h"
#include "Platform/Time.h"
#include "Platform/Window.h"
#include "Renderer/IRenderer.h"

#include <cmath>
#include <memory>

int main() {
    std::unique_ptr<Engine::Window> window =
        Engine::Window::Create({"Voxel Engine - Sandbox", 1280, 720});
    Engine::Input::Init(window.get());

    std::unique_ptr<Engine::IRenderer> renderer = Engine::IRenderer::Create(window.get());

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

        Engine::Camera cam{};
        renderer->BeginFrame(cam);
        renderer->EndFrame();

        window->SwapBuffers();
    }

    return 0;
}
