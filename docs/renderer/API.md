# Renderer — Proposed API

**Status: proposed, not implemented.** This is the shape the interface is
expected to take once `docs/renderer/ROADMAP.md`'s sequence is built. It
will be refined during implementation — unlike `EngineMath`, this is
explicitly *not* frozen yet (see `docs/decisions/0008-api-stability-policy.md`:
freezing happens after an API review of a working implementation, not
before one exists). Treat every signature below as a working hypothesis to
validate against real usage, not a commitment.

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

Open questions to resolve during implementation (not decided yet — these
are exactly what building `OpenGLRenderer` will answer):

- Does `CreateMesh`/`CreateShader`/`CreateTexture` need a destroy/release
  counterpart now, or can that wait until something actually needs to free
  GPU memory at runtime (Phase 3's Sandbox likely creates a fixed set of
  resources once and never frees them)?
- Does `Submit` need a return value / error signal for an invalid handle,
  or is that an `ENGINE_ASSERT` in debug builds (consistent with how
  `EngineMath` handles invalid operations)?
- Is `RenderCommand` batched/sorted internally by the Renderer (by
  shader, to minimize state changes), or is that the caller's
  responsibility? Leaning toward "Renderer's responsibility, caller
  doesn't need to know" — but not committing until Phase 3's "Render
  Queue" step (see ROADMAP.md) is actually built.

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
