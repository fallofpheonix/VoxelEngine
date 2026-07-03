#include "Platform/Input.h"
#include "Platform/Window.h"

#include "Foundation/Assert.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Engine {

namespace {
GLFWwindow* g_activeWindow = nullptr;

int ToGLFWKey(Key key) {
    switch (key) {
        case Key::W:      return GLFW_KEY_W;
        case Key::A:      return GLFW_KEY_A;
        case Key::S:      return GLFW_KEY_S;
        case Key::D:      return GLFW_KEY_D;
        case Key::Up:     return GLFW_KEY_UP;
        case Key::Down:   return GLFW_KEY_DOWN;
        case Key::Left:   return GLFW_KEY_LEFT;
        case Key::Right:  return GLFW_KEY_RIGHT;
        case Key::Space:  return GLFW_KEY_SPACE;
        case Key::Escape: return GLFW_KEY_ESCAPE;
    }
    return GLFW_KEY_UNKNOWN;
}
} // namespace

void Input::Init(Window* window) {
    g_activeWindow = static_cast<GLFWwindow*>(window->GetNativeHandle());
}

bool Input::IsKeyPressed(Key key) {
    ENGINE_ASSERT(g_activeWindow, "Input::Init must be called before querying input");
    return glfwGetKey(g_activeWindow, ToGLFWKey(key)) == GLFW_PRESS;
}

} // namespace Engine
