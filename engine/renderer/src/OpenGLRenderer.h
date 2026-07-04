#pragma once

#include "Renderer/IRenderer.h"

namespace Engine {

class OpenGLRenderer : public IRenderer {
public:
    explicit OpenGLRenderer(Window* window);
    ~OpenGLRenderer() override = default;

    MeshHandle CreateMesh(const MeshData& data) override;
    ShaderHandle CreateShader(const std::string& vertexSrc, const std::string& fragmentSrc) override;
    TextureHandle CreateTexture(int width, int height, const uint8_t* rgbaPixels) override;

    void BeginFrame(const Camera& camera) override;
    void Submit(const RenderCommand& command) override;
    void EndFrame() override;

private:
    Window* m_window = nullptr;
};

} // namespace Engine
