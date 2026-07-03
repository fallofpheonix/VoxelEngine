#include "Platform/Window.h"

#include "Foundation/Assert.h"
#include "Foundation/Log.h"

#include <GL/glew.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Engine {

namespace {
int g_glfwWindowCount = 0;

void GLFWErrorCallback(int error, const char* description) {
    ENGINE_LOG_ERROR(std::string("GLFW error (") + std::to_string(error) + "): " + description);
}
} // namespace

class GLFWWindowImpl final : public Window {
public:
    explicit GLFWWindowImpl(const WindowProps& props) : m_width(props.width), m_height(props.height) {
        if (g_glfwWindowCount == 0) {
            const int ok = glfwInit();
            ENGINE_ASSERT(ok, "Failed to initialize GLFW");
            glfwSetErrorCallback(GLFWErrorCallback);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); // required on macOS

        m_handle = glfwCreateWindow(props.width, props.height, props.title.c_str(), nullptr, nullptr);
        ENGINE_ASSERT(m_handle, "Failed to create GLFW window");
        ++g_glfwWindowCount;

        glfwMakeContextCurrent(m_handle);
        glfwSwapInterval(1); // vsync -> stable FPS tied to display refresh

        glewExperimental = GL_TRUE;
        const GLenum glewStatus = glewInit();
        ENGINE_ASSERT(glewStatus == GLEW_OK, "Failed to initialize GLEW");

        glfwSetWindowUserPointer(m_handle, this);
        glfwSetFramebufferSizeCallback(m_handle, [](GLFWwindow* window, int width, int height) {
            auto* self = static_cast<GLFWWindowImpl*>(glfwGetWindowUserPointer(window));
            self->m_width = width;
            self->m_height = height;
            glViewport(0, 0, width, height);
        });

        ENGINE_LOG_INFO(std::string("OpenGL context: ") +
                         reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    }

    ~GLFWWindowImpl() override {
        glfwDestroyWindow(m_handle);
        --g_glfwWindowCount;
        if (g_glfwWindowCount == 0) {
            glfwTerminate();
        }
    }

    void PollEvents() override { glfwPollEvents(); }
    void SwapBuffers() override { glfwSwapBuffers(m_handle); }
    bool ShouldClose() const override { return glfwWindowShouldClose(m_handle); }

    int GetWidth() const override { return m_width; }
    int GetHeight() const override { return m_height; }

    void* GetNativeHandle() const override { return m_handle; }

private:
    GLFWwindow* m_handle = nullptr;
    int m_width;
    int m_height;
};

std::unique_ptr<Window> Window::Create(const WindowProps& props) {
    return std::make_unique<GLFWWindowImpl>(props);
}

} // namespace Engine
