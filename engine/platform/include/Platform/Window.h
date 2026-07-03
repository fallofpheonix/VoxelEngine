#pragma once

#include <memory>
#include <string>

namespace Engine {

struct WindowProps {
    std::string title = "Voxel Engine";
    int width = 1280;
    int height = 720;
};

// Engine code talks to this interface only. GLFW (or any future backend)
// is confined to the backend's .cpp file.
class Window {
public:
    virtual ~Window() = default;

    virtual void PollEvents() = 0;
    virtual void SwapBuffers() = 0;
    virtual bool ShouldClose() const = 0;

    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;

    virtual void* GetNativeHandle() const = 0;

    static std::unique_ptr<Window> Create(const WindowProps& props = {});
};

} // namespace Engine
