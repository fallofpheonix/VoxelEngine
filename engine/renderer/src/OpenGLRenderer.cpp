#include "OpenGLRenderer.h"

#include "Foundation/Assert.h"
#include "Foundation/Log.h"
#include "Platform/Window.h"

#include <GL/glew.h>

namespace Engine {

OpenGLRenderer::OpenGLRenderer(Window* window) : m_window(window) {
    ENGINE_ASSERT(m_window != nullptr, "OpenGLRenderer: window must not be null");

    const auto* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    ENGINE_ASSERT(version != nullptr, "OpenGLRenderer: failed to query GL_VERSION -- is a GL context current?");
    ENGINE_LOG_INFO(std::string("OpenGLRenderer initialized: ") + version);
}

MeshHandle OpenGLRenderer::CreateMesh(const MeshData& /*data*/) {
    return {};
}

ShaderHandle OpenGLRenderer::CreateShader(const std::string& /*vertexSrc*/, const std::string& /*fragmentSrc*/) {
    return {};
}

TextureHandle OpenGLRenderer::CreateTexture(int /*width*/, int /*height*/, const uint8_t* /*rgbaPixels*/) {
    return {};
}

void OpenGLRenderer::BeginFrame(const Camera& /*camera*/) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::Submit(const RenderCommand& /*command*/) {
}

void OpenGLRenderer::EndFrame() {
}

std::unique_ptr<IRenderer> IRenderer::Create(Window* window) {
    return std::make_unique<OpenGLRenderer>(window);
}

} // namespace Engine
