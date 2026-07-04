# Renderer — API

**Status: implemented, Stable.** This document reflects the actual built
interface after completing `docs/renderer/ROADMAP.md`'s 13-step sequence.
See `docs/decisions/0008-api-stability-policy.md` for the API classification.

## Handles

Opaque, not GL object IDs — see `DESIGN.md`'s "what stays internal."

```cpp
struct ShaderHandle  { uint32_t id = 0; bool IsValid() const { return id != 0; } };
struct MeshHandle    { uint32_t id = 0; bool IsValid() const { return id != 0; } };
struct TextureHandle { uint32_t id = 0; bool IsValid() const { return id != 0; } };
```

## Data types (backend-independent)

```cpp
struct Vertex {
    Vector3 position;
    Vector3 normal;
    Vector2 uv;
};

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

struct Material {
    ShaderHandle shader;
    TextureHandle texture;   // single texture for Phase 3; a texture *set*
                              // is a Phase 10 (Materials/PBR) concern
    Vector3 color{1.0f, 1.0f, 1.0f};  // solid color tint, set via uColor uniform
};

struct Camera {
    Matrix4 view;
    Matrix4 projection;

    static Camera Perspective(const Vector3& eye, const Vector3& target,
                               float fovYRadians, float aspect, float nearZ, float farZ);
};

struct RenderCommand {
    MeshHandle mesh;
    Material material;
    Matrix4 modelMatrix;   // from Transform::ToMatrix4()
};
```

`Camera` lives in the Renderer module, not `EngineMath` — it's a rendering
concept (view/projection composition, tied to a specific frame) built from
Math primitives, not a math primitive itself. This mirrors how `Transform`
is Math (pure TRS composition) but `Camera` is not.

## IRenderer

```cpp
class IRenderer {
public:
    virtual ~IRenderer() = default;

    // Resource creation. Called once per resource, not per frame.
    virtual MeshHandle CreateMesh(const MeshData& data) = 0;
    virtual ShaderHandle CreateShader(const std::string& vertexSrc,
                                       const std::string& fragmentSrc) = 0;
    virtual TextureHandle CreateTexture(int width, int height,
                                         const uint8_t* rgbaPixels) = 0;

    // Frame submission.
    virtual void BeginFrame(const Camera& camera) = 0;
    virtual void Submit(const RenderCommand& command) = 0;
    virtual void EndFrame() = 0;

    // Mirrors Window::Create(WindowProps) exactly -- see
    // engine/platform/include/Platform/Window.h.
    static std::unique_ptr<IRenderer> Create(Window* window);
};
```

## Resolved design decisions

These were open questions during design (see the original proposed version
of this document). They are resolved after building the implementation:

- **Destroy/release counterpart**: Deferred. The Sandbox creates a fixed
  set of resources once and never frees them. A `DestroyMesh`/`DestroyShader`
  API will be added when a concrete consumer (e.g., an Asset Pipeline or
  hot-reload system) needs to free GPU memory at runtime.

- **Submit error handling**: Invalid handles are handled via
  `ENGINE_ASSERT` in debug builds, consistent with how `EngineMath` handles
  invalid operations (e.g., dividing by zero, normalizing a zero-length
  vector). In Release builds, invalid handles are silently skipped.

- **Submit batching/sorting**: Immediate mode. `Submit` issues a draw call
  immediately. No internal batching or sorting. This is sufficient for
  Phase 3's usage patterns. A render queue with batching can be added later
  if profiling shows the immediate approach is a bottleneck with many
  objects.

## What's explicitly not in this API

- No `Draw()`-per-primitive immediate-mode calls — the old tutorial's
  `ImmediateVertex`-style API (see the archived
  `legacy_voxel_tutorial/src/SingleCube.cpp`) is exactly what this
  replaces. Everything goes through `CreateMesh` + `Submit`.
- No lighting/material-parameter uniforms beyond a single texture — see
  DESIGN.md's non-goals.
- No multi-backend selection at this API layer — `IRenderer::Create`
  always returns an `OpenGLRenderer` today; a backend parameter isn't
  added until a second backend exists (per the two-implementations rule).
