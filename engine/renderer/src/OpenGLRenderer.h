#pragma once

#include "Renderer/IRenderer.h"

#include <vector>

namespace Engine {

struct MeshGL {
    unsigned int vao = 0;
    unsigned int vbo = 0;
    unsigned int ebo = 0;
    unsigned int indexCount = 0;
};

class OpenGLRenderer : public IRenderer {
public:
    explicit OpenGLRenderer(Window* window);
    ~OpenGLRenderer() override;

    MeshHandle CreateMesh(const MeshData& data) override;
    ShaderHandle CreateShader(const std::string& vertexSrc, const std::string& fragmentSrc) override;
    TextureHandle CreateTexture(int width, int height, const uint8_t* rgbaPixels) override;

    void BeginFrame(const Camera& camera) override;
    void Submit(const RenderCommand& command) override;
    void EndFrame() override;

private:
    Window* m_window = nullptr;

    std::vector<unsigned int> m_shaders;
    std::vector<MeshGL> m_meshes;
    std::vector<unsigned int> m_textures;

    unsigned int m_currentShader = 0;
};

} // namespace Engine
