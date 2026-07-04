#pragma once

#include <cstdint>

namespace Engine {

struct ShaderHandle {
    uint32_t id = 0;
    bool IsValid() const { return id != 0; }
};

struct MeshHandle {
    uint32_t id = 0;
    bool IsValid() const { return id != 0; }
};

struct TextureHandle {
    uint32_t id = 0;
    bool IsValid() const { return id != 0; }
};

} // namespace Engine
