#pragma once

#include "Renderer/Handles.h"
#include "Renderer/Types.h"

#include <memory>
#include <string>

namespace Engine {

class Window;

class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual MeshHandle CreateMesh(const MeshData& data) = 0;
    virtual ShaderHandle CreateShader(const std::string& vertexSrc, const std::string& fragmentSrc) = 0;
    virtual TextureHandle CreateTexture(int width, int height, const uint8_t* rgbaPixels) = 0;

    virtual void BeginFrame(const Camera& camera) = 0;
    virtual void Submit(const RenderCommand& command) = 0;
    virtual void EndFrame() = 0;

    static std::unique_ptr<IRenderer> Create(Window* window);
};

} // namespace Engine
