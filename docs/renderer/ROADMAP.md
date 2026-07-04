# Renderer — Roadmap

Sequence from `docs/architecture/ARCHITECTURE.md`'s "Renderer (next phase)"
section, expanded into concrete steps with quality gates, following the
Phase 2 template (`ARCHITECTURE.md`'s "Quality gates" section). Each step
should be a small, buildable increment — not one large PR for the whole
Renderer.

Interface first (`IRenderer`, `API.md`), OpenGL implementation last —
within this roadmap, not just across it. A step is only "done" when its
gate passes; don't move to the next step with a gate item unchecked
without explicitly deferring it (as Phase 2 did with SIMD benchmarks) and
saying why.

```text
[x] 1. Graphics Context
        Deliverable: OpenGLRenderer can be constructed from a Window and
        confirms a valid GL context (reuses the context Window already
        creates -- engine/platform/src/GLFWWindow.cpp -- doesn't create
        a second one).
        Gate: [x] Sandbox constructs an IRenderer with no window/context changes
              [x] Zero compiler warnings
              [x] Headless-safe: constructing without calling any draw method
                  doesn't crash (partial test of the "rendering is optional"
                  invariant in ARCHITECTURE.md)

[x] 2. Shader
        Deliverable: CreateShader compiles + links a GLSL vertex/fragment
        pair, returns a ShaderHandle, surfaces compile/link errors via
        Foundation's Log (ENGINE_LOG_ERROR), not by crashing.
        Gate: [x] Unit or smoke test: valid shader source succeeds
              [x] Deliberately broken shader source logs an error and
                  returns an invalid handle, doesn't crash
              [x] Zero compiler warnings

[x] 3. Buffer + Vertex Array
        Deliverable: internal-only (no new public API) -- VBO/EBO/VAO
        creation and binding, driven by CreateMesh in step 4.
        Gate: [x] No public surface added (this step is Internal per
                  docs/decisions/0008-api-stability-policy.md's classification)

[x] 4. Mesh
        Deliverable: CreateMesh(MeshData) uploads vertex/index data,
        returns a MeshHandle.
        Gate: [x] A hardcoded triangle's MeshData round-trips through
                  CreateMesh without error
              [x] Zero compiler warnings

[x] 5. Texture
        Deliverable: CreateTexture(width, height, rgbaPixels) uploads a
        texture, returns a TextureHandle.
        Gate: [x] A solid-color in-memory pixel buffer uploads successfully
              [x] Zero compiler warnings
        Note: no image file loading yet (PNG/JPG decoding is Phase 7,
              Asset Pipeline) -- this step only proves the GL upload path
              works given raw pixels already in memory.

[x] 6. Material
        Deliverable: Material{ShaderHandle, TextureHandle} as a plain
        struct (API.md) -- no new Renderer methods, just the data type
        RenderCommand will carry.
        Gate: [x] Compiles, no behavior yet to test independently

[x] 7. Camera
        Deliverable: Camera struct + Camera::Perspective(...) helper,
        built on EngineMath's existing Matrix4::Perspective/LookAt (Stable,
        v1.0 -- no changes needed to Math for this step).
        Gate: [x] Unit test: Camera::Perspective produces the same matrices
                  as calling Matrix4::Perspective/LookAt directly (this
                  step is a thin convenience wrapper, not new math)

[x] 8. Render Queue
        Deliverable: BeginFrame(Camera) / Submit(RenderCommand) / EndFrame()
        on OpenGLRenderer -- the first version that actually issues draw
        calls.
        Gate: [x] Submitting zero commands between BeginFrame/EndFrame
                  doesn't crash (headless-safe)
              [x] Resolves the open API question in API.md: does Submit
                  batch/sort by shader, or is it immediate? Decide here,
                  document the answer in API.md, don't leave it open past
                  this step.

[x] 9. OpenGLRenderer (assembly)
        Deliverable: all of the above wired together behind IRenderer;
        this is the point IRenderer::Create(Window*) becomes real instead
        of a declared-but-unimplemented static method.
        Gate: [x] Sandbox replaces its raw glClear/glClearColor calls
                  (sandbox/src/main.cpp) with IRenderer calls
              [x] Zero compiler warnings
              [x] No GL types (GLuint, etc.) appear outside
                  engine/renderer/src/OpenGL*.cpp -- verify with a grep,
                  matching how GLFWWindow.cpp is the only file touching GLFW

--- content-level milestones, only after step 9 ---

[x] 10. Triangle    Sandbox renders one hardcoded triangle via CreateMesh + Submit
[x] 11. Cube        Sandbox renders a cube -- this is the point the archived
                     legacy_voxel_tutorial/src/SingleCube.cpp's concept is
                     finally real instead of console-mocked
[x] 12. Model       Load a mesh from something other than a hardcoded array
                     (still no file format yet -- Phase 7, Asset Pipeline,
                     owns actual OBJ/glTF loading; this step just proves
                     CreateMesh handles non-trivial vertex counts)
[x] 13. Scene       Multiple objects, multiple materials, submitted in one
                     frame -- the point at which "does Submit need batching"
                     from step 8 either mattered or didn't
```

## Definition of done for the Renderer phase overall

Mirrors Phase 2's quality gate shape:

```text
[x] Unit/smoke tests for each step above
[x] Documentation: this ROADMAP.md's checkboxes complete, API.md's open
    questions resolved (not left as "TBD")
[x] Zero compiler warnings
[x] No circular dependencies (Renderer depends on Core + Platform only,
    never the reverse -- verify against docs/architecture/ARCHITECTURE.md's
    "Module dependency declarations" format)
[x] API review completed (same process as Phase 2's, before declaring
    Renderer Stable per docs/decisions/0008-api-stability-policy.md)
```

Once that passes, the Renderer gets its own frozen-version declaration
(a "Renderer v1.0" decision record), the same way Math did.
