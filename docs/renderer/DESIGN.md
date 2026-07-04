# Renderer — Design

**Status: implemented, Stable.** This document, `API.md`, and
`ROADMAP.md` were written before any `engine/renderer/` code existed, per
`docs/architecture/ARCHITECTURE.md`'s "build only when two implementations
exist" rule applied at the design level: understand the shape of the
single (OpenGL) implementation before writing it, so the interface isn't
just OpenGL with the names changed. All 13 roadmap steps are complete.

## What does the Renderer own?

GPU-resident resources and the state needed to submit draw calls:

- Compiled shader programs
- Vertex/index buffers and their vertex array bindings
- Textures
- The current frame's render queue (built up between `BeginFrame`/`EndFrame`,
  discarded after)

It does **not** own: the window or GL context (that's `Engine::Window`,
already built in `engine/platform`), scene/entity data (future `Scene`
module), or asset files on disk (future `Resources` module). The Renderer
consumes handles and data from those; it doesn't reach up to fetch them.

This matches the Ownership rule already stated in
`docs/architecture/ARCHITECTURE.md`: "Renderer owns GPU resources."

## What does it consume (inputs)?

- **Mesh data** — vertex/index arrays, handed to the Renderer once at
  creation time (`CreateMesh`), not per-frame.
- **Material** — a shader handle plus texture handle(s) and parameter
  values.
- **Camera** — view and projection matrices. Built from `Engine::Matrix4`
  (`Perspective`, `Orthographic`, `LookAt` already exist in `EngineMath` —
  see `docs/decisions/0008-api-stability-policy.md`, those are Stable and
  won't change under the Renderer).
- **Transform** — per-object model matrix, from `Engine::Transform::ToMatrix4()`.

None of these are GL types at the call site. See API.md for the exact
shapes.

## What does it expose (outputs)?

A single interface, `IRenderer`, covering:

- Resource creation (`CreateMesh`, `CreateShader`, `CreateTexture`) —
  returns opaque handles, not GL object IDs.
- Frame submission (`BeginFrame`, `Submit`, `EndFrame`).

Nothing else is public. A caller (Sandbox, eventually a Scene module)
never touches a `GLuint`, a shader source compilation error string format,
or a VAO binding order — those are Renderer-internal.

## Threading

**Main thread only, for now.** `Engine::Window`'s contract (see
`docs/architecture/ARCHITECTURE.md`'s Platform contract) already
constrains the GL context to the main thread — GLFW's context is only
current on the thread that created it. A dedicated render thread is a
real future need (see the original 35-phase roadmap's "Multithreading"
phase) but building one now, with a single-threaded main loop and no
profiling data showing a bottleneck, would be exactly the kind of
speculative infrastructure this project avoids. Revisit when Physics or
a heavier scene genuinely contends with rendering for main-thread time.

## Backend-independent vs. backend-specific

Same split as `Window`/`GLFWWindowImpl`:

```text
Backend-independent (engine/renderer/include/Renderer/*.h)
    IRenderer, MeshData, Material, Camera, RenderCommand
        — pure data + the interface, zero GL includes

Backend-specific (engine/renderer/src/OpenGL*.cpp)
    OpenGLRenderer : IRenderer
        — the only files that #include <GL/glew.h>
```

`IRenderer::Create(Window*)` is the single factory function, mirroring
`Window::Create()` exactly. No plugin registry, no factory pattern beyond
that one function — per the "build only when two implementations exist"
rule, a second backend (Vulkan/Metal) doesn't justify more machinery than
this until it actually exists.

## What stays internal

- Shader compilation/linking (GLSL source handling, compile error
  formatting, uniform location caching).
- Buffer upload strategy (single VBO vs. per-mesh VBOs, static vs. dynamic
  usage hints).
- VAO attribute binding layout.
- The concrete meaning of a handle's integer value (index into an internal
  resource table — never a raw `GLuint` exposed to callers, so swapping
  backends later doesn't leak GL semantics into calling code even
  accidentally).

Per `docs/decisions/0008-api-stability-policy.md`'s Stable/Deprecated/
Internal classification, all of the above is **Internal** from day one —
it was never intended to be part of the Renderer's public surface, so
there's no future "should this become private" debate to have.

## Explicit non-goals for this phase

Deferred, not forgotten — each has a future roadmap slot
(`docs/architecture/ARCHITECTURE.md`'s roadmap):

- Lighting, shadows, PBR materials — Phase 9/10 territory in the original
  roadmap, not Phase 3.
- Multiple render passes / a render graph — the "Future Renderer Layers"
  diagram in `ARCHITECTURE.md` names this, but a render graph solves a
  problem (managing pass dependencies) this phase doesn't have yet with a
  single forward-rendered scene.
- Multiple backends — OpenGL only. Vulkan/Metal wait for a concrete need
  per the two-implementations rule.
- Frustum/occlusion culling — needs a Scene Graph to cull against, which
  doesn't exist yet (next phase after Renderer).

## Relationship to existing modules

```text
Sandbox
   │ owns
   ▼
Engine::Window  (existing, engine/platform)
   │ provides GL context via GetNativeHandle()
   ▼
IRenderer::Create(Window*)
   │ consumes
   ▼
Engine::Matrix4 / Vector3 / Transform  (existing, engine/math, frozen v1.0)
```

The Renderer is the first Engine Module — the first thing allowed to
depend on both Core (Math) and Platform, per the layer diagram in
`docs/architecture/ARCHITECTURE.md`.
