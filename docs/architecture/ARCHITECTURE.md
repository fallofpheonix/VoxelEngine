# Architecture

**Version 0.4** — see Changelog at the bottom for what changed since 0.3.

**Governance note (as of 0.3):** this document has reached a stable
baseline (Foundation → Platform → Math are done and reviewed). From here,
it should change only for genuinely architectural decisions — new layers,
new invariants, changes to the dependency rule. Subsystem-level design
(how the Renderer's command buffer works, what the Physics broadphase
does) belongs in `docs/<module>/DESIGN.md`, created when that module
starts, not appended here. This file stays a reference, not a running log
of everything being built.

## Identity

A modular, AI-native, data-oriented engine platform where the core stays
small, every subsystem is replaceable, and the same architecture can power
games, simulations, robotics, visualization, dedicated servers, and voxel
worlds.

This is not an attempt to out-feature Unity or Godot. Those are mature game
engines built around a single application model (editor → game). This
project is an engine *platform*: one core, many possible applications.

```text
Engine
  ├── Game
  ├── Simulation
  ├── Robotics
  ├── Visualization
  ├── AI Training
  └── Dedicated Server
```

## The single invariant that matters most

> **Dependencies always point downward. Lower layers must never depend on
> higher layers. No exceptions.**

Every other rule below exists to protect this one. The biggest risk to a
multi-year engine is not missing features — it's architectural drift:
dependencies quietly pointing the wrong way until every subsystem is
entangled with every other one.

## Layers

```text
Applications
      │
      ▼
Engine Modules   (Renderer, Physics, Audio, Scene, Animation, Networking, Resources, Scripting)
      │
      ▼
Core             (Math, Memory, Jobs, Containers, Events   [ECS, Reflection, Serialization: deferred — see below])
      │             Platform (Window, Input, Filesystem, Time, OS Services)
      ▼             │
      └──────┬───────┘
             ▼
        Foundation  (Log, Assert, Types, Result, UUID, Hash, Config, Build Config, Platform Detection)
```

| Layer | Stability | Purpose |
|---|---|---|
| Foundation | Highest | Zero-dependency infrastructure used by everything (Log, Assert, ...) |
| Core | Very high | Foundation-only engine logic (Math, Memory, Jobs, Containers, ...) |
| Platform | Medium | OS/hardware adapter (Window, Input, Time, Filesystem) |
| Engine Modules | Medium | Rendering, Physics, Audio, Scene, Animation, Networking, Resources, Scripting |
| Applications | Low | Games, Editor, Tools, Server, Sandbox |

Foundation depends on nothing. Core depends only on Foundation. Platform
depends only on Foundation. Core and Platform never depend on each other —
this replaced an earlier "Core/Platform are peers with a Log exception"
rule; see `docs/decisions/0003-layering.md` for why. Engine Modules depend
on Core and/or Platform. Applications depend on Engine Modules.

**Foundation's scope is deliberately narrow**: only utilities that could
reasonably appear in *any* C++ project (Log, Assert, Types, Result/Expected,
UUID, Hash, Config, build/platform detection). Math, memory allocators,
containers, reflection, ECS, and serialization are Core, not Foundation —
they're engine-specific even if some are still unbuilt.

**Reflection and ECS are intentionally not implemented yet**, despite being
listed under Core above as their eventual home. Building either before
there's a concrete consumer produces speculative API surface:

- Reflection becomes worth its complexity once at least two of
  {Editor, Serialization, Scripting, Networking replication} exist and
  need shared metadata. None do yet.
- ECS becomes worth its complexity once there's a Renderer and a Scene
  Graph to observe actual data-flow patterns against. Designing an ECS
  before that means guessing at access patterns instead of measuring them.
  See the Roadmap section for the planned checkpoint (a simple entity
  system first, ECS evaluated only after).

Current code:

```text
EngineFoundation   (engine/foundation)  — Log, Assert. No dependencies.
EngineMath         (engine/math)        — Vector2/3/4, Matrix3/4, Quaternion,
                                           Transform, Ray, Plane, AABB, Sphere,
                                           Intersections. Depends on EngineFoundation only.
EnginePlatform     (engine/platform)    — Window, Input, Time. Depends on EngineFoundation only.
Sandbox            (sandbox)            — Application. Depends on EngineFoundation, EngineMath, EnginePlatform.
```

## Build only when two implementations exist

Don't build an abstraction for an imagined future backend. Build the
concrete thing first.

```text
Window
  ↓
GLFWWindow          # first implementation: no factory, no plugin system,
                     # just an interface + one backend
```

```text
Window
  ↓
GLFWWindow
SDLWindow            # second real implementation appears: now the
                     # abstraction has proven its worth
```

```text
Window
  ↓
Dynamic Backend Loader   # only once swapping backends at runtime is an
                          # actual requirement, not a hypothetical one
```

Applies to Renderer (OpenGL/Vulkan/Metal), Physics (Bullet/Jolt), Audio
(OpenAL/FMOD), and everything else. Precedent already in the codebase:
`engine/platform/include/Platform/Window.h` defines the interface;
`engine/platform/src/GLFWWindow.cpp` is the only file that touches GLFW.

Before adding any subsystem, ask:

1. Is there a concrete problem today?
2. Will at least two independent modules benefit from this abstraction?
3. Does this reduce long-term coupling rather than increase it?

If any answer is "no," defer it. This explicitly rules out building —
until a real need forces the issue — a plugin manager, reflection system,
asset database, hot reload, package manager, or marketplace.

## Interfaces are split from implementations

Every subsystem with a backend follows the same shape: a public interface
(and the data types it exchanges) separate from the concrete backend(s)
implementing it.

```text
Renderer

Interface
    IRenderer, Mesh, Material, Camera
Implementations
    OpenGLRenderer, VulkanRenderer, MetalRenderer
```

Already true of Platform today:

```text
Window (interface, engine/platform/include/Platform/Window.h)
    ↓
GLFWWindowImpl (implementation, engine/platform/src/GLFWWindow.cpp)
```

## Ownership

Every piece of engine data has exactly one owner. Other subsystems may
reference it, but ownership is never shared unless a concrete requirement
forces it (e.g. a future job system's double-buffered resources).

```text
Renderer   owns GPU resources
Physics    owns the collision world
Scene      owns the entity hierarchy
Animation  owns animation graphs
```

## Module dependency declarations

Every module states three things: what it requires, what it optionally
integrates with, and what it must never depend on. This is enforced by
review today; see "Architectural metrics" below for when this becomes
automatable.

```text
Renderer

Required
    Core
    Platform
Optional
    Physics        (e.g. debug-draw collision shapes)
Forbidden
    Gameplay
    Editor
```

## Engine contracts

Each subsystem states, in a few lines: inputs, outputs, owning thread,
dependencies, and explicit non-dependencies.

Current contracts:

```text
Foundation (Log / Assert)

Input
    A LogLevel and a message string
Output
    Formatted, flushed output to stdout/stderr
Thread
    Any (no shared mutable state)
Depends On
    Nothing
Never Depends On
    Core, Platform, Engine Modules, Applications
```

```text
Platform (Window / Input / Time)

Input
    OS window/input events
Output
    Window, Input, Time interfaces (Engine::Window, Engine::Input, Engine::Time)
Thread
    Main thread only
Depends On
    Foundation
Never Depends On
    Core, Engine Modules, Applications
```

Target shape for the eventual Renderer (not yet built):

```text
Renderer

Input
    Meshes, Materials, Camera
Output
    GPU Commands
Thread
    Render Thread
Depends On
    Core, Platform
Never Depends On
    Gameplay, AI
```

## Module manifest (future — not yet needed)

Once there are more than a handful of modules, each should expose metadata
like:

```text
Module: Renderer
Version: 1.0
Thread Safe: Partial
Dependencies: Core, Platform
Public Interfaces: IRenderer
Implementation: OpenGLRenderer
Status: Stable
```

Not built yet — one or two modules don't need a metadata system to stay
comprehensible. Revisit once Engine Modules has enough entries that
`ARCHITECTURE.md`'s prose stops being sufficient (per the "build only when
there's a concrete problem" rule).

## Architecture invariants (review checklist)

- **Dependency**: higher layers depend on lower layers, never the reverse.
- **Ownership**: every object has exactly one owner; avoid shared ownership
  unless required.
- **Threading**: only the owner mutates data; worker threads receive jobs.
- **Memory**: allocation is explicit; avoid hidden heap allocations.
- **API**: interfaces stay stable; implementations evolve underneath them.
- **Determinism**: simulation produces identical results given identical
  inputs.
- **Modules**: communicate through interfaces; never include another
  module's implementation headers.
- **Build**: every module compiles independently.
- **Platform**: platform code never leaks into engine or core code.
- **Rendering**: optional — headless mode stays supported.
- **Testing**: every subsystem is testable in isolation.
- **Performance**: no heap allocation inside a render loop, physics update,
  animation update, or job execution. `EngineMath` satisfies this by
  construction — every type is a stack value with no internal allocation.

## Architectural metrics (future — not yet enforced)

Once there's a CI pipeline (there isn't one yet — no remote repo exists),
these should be checked automatically rather than by review:

```text
Maximum dependency depth
Maximum compile dependencies per module
Cycles allowed: 0
Layer violations: 0
Forbidden includes: 0   (e.g. Foundation must never #include a Platform header)
```

## Quality gates

A phase isn't "done" until its gate passes — this exists to stop phases
from staying "mostly done" indefinitely.

```text
Phase 2 — Math

[x] Unit tests           33 test cases / 93 assertions, tests/math/, run via CTest
[ ] SIMD benchmarks       Deferred — nothing has profiled a math bottleneck yet;
                          scalar is correct and simple. Revisit only if profiling
                          (once a Renderer/Physics loop exists) shows Math as a
                          hot path. Building SIMD now would be optimizing
                          without a measurement.
[x] Documentation         engine/math/README.md — conventions (column-major,
                          right-handed, radians, NDC depth range, no hidden allocation,
                          fixed world directions, no operator==, no implicit conversions)
[x] Zero compiler warnings  Built clean with -Wall -Wextra
[x] No circular dependencies  Single module, depends only on EngineFoundation
[x] API review completed  See "Phase 2 API review" below — findings applied
```

Template for future phases: copy this block, swap the checklist items for
what that phase's deliverable actually needs (e.g. Renderer's gate should
include "renders a textured cube" and "headless mode still builds," not
SIMD benchmarks).

### Phase 2 API review

Findings and resolutions, since this is an API meant to stay stable for
years once frozen:

| Finding | Resolution |
|---|---|
| Exact `operator==` on `Vector2/3/4` invites float-equality bugs | Removed; replaced with `.NearlyEqual(other, epsilon)`. `Matrix4` got the same treatment (it never had `operator==`, but gained `NearlyEqual` to replace a test-local helper). See `docs/decisions/0007-floating-point-equality.md`. |
| Constants were sparse (`kPi` only) | Added `kTwoPi`, `kHalfPi`, `kEpsilon`, `kInfinity`, `kNegativeInfinity` to `Constants.h`. |
| No fixed world-direction vocabulary | Added `Vector3::Forward/Back/Right/Left/Up/Down()`, documented in `engine/math/README.md` with the exact axis mapping (`Forward = -Z`). |
| Matrix convention (row vs. column vectors) was implicit in the code, not stated | Documented explicitly in `engine/math/README.md`: column-vector convention, `M * v` only, no `operator*(Vector4, Matrix4)`. |
| `Transform` composition order wasn't called out as permanent | `engine/math/README.md` now states `T * R * S` as a fixed API commitment, not a default. |
| Possible implicit conversions | Audited every constructor in `engine/math`; none take a single argument except copy/move. The one converting constructor, `Vector4(const Vector3&, float)`, is now `explicit`. |
| Determinism across platforms | Not solved (would mean fixed-point math with no current consumer). Documented as a known limitation in `engine/math/README.md`, with the trigger for revisiting it (Networking rollback/prediction) stated explicitly. |
| `Ray`/`Plane`/`AABB`/`Sphere`/`Intersections` living inside Math long-term | Left as-is for now (5 small files, no current pain). Documented the planned split into `engine/geometry` and its trigger (adding `OBB`, `Frustum`, `Triangle`, or `Capsule`) in `engine/math/README.md`. |

## Roadmap

```text
Foundation      done
      ↓
Platform        done (Window/Input/Time via GLFW backend)
      ↓
Math            done (this phase)
      ↓
Renderer
      ↓
Scene Graph
      ↓
Asset System
      ↓
Physics
      ↓
Animation
      ↓
Audio
      ↓
Networking
      ↓
Scripting
      ↓
Editor
      ↓
AI Tools
      ↓
Plugin Infrastructure
```

Reflection, ECS, plugin loading, a package manager, and a marketplace are
all intentionally absent from this list — each is deferred until a
concrete consumer forces the issue (see "Reflection and ECS are
intentionally not implemented yet" above, and the "build only when two
implementations exist" rule).

**The Editor is deliberately late**, after Renderer through Scripting, not
early. An editor built before there are proven workflows to support ends
up encoding guesses instead of validated needs — the Editor should emerge
from having built a Sandbox and several small games/demos against the
engine, not precede them.

### Renderer (next phase) — planned sequence, not started

Recorded here so the plan survives even though implementation hasn't
begun. Interfaces first, OpenGL second — same "build only when two
implementations exist" rule applies to *within* the Renderer, not just
across backends:

```text
Renderer interface (IRenderer)
      ↓
Graphics Context
      ↓
Shader
      ↓
Buffer
      ↓
Vertex Array
      ↓
Texture
      ↓
Mesh
      ↓
Material
      ↓
Camera
      ↓
Render Queue
      ↓
Renderer
```

Only once that's in place does content move up the stack:

```text
Triangle → Cube → Model → Scene
```

Gameplay never reaches OpenGL directly — it goes through the Render
System, which goes through a Render Graph, which goes through the
Renderer API, which goes through the OpenGL backend:

```text
Application → Render System → Render Graph → Renderer API → OpenGL Backend → OpenGL Driver
```

This is a large phase — worth its own `docs/renderer/DESIGN.md` once
started, per the governance note at the top of this file, rather than
being expanded here.

## Decision log

Architectural decisions with real alternatives considered are recorded in
`docs/decisions/` (one file per decision, numbered). This prevents
re-litigating settled questions. Current entries:

- `0001-window-library.md` — GLFW over SDL2/native
- `0002-build-system.md` — CMake + vcpkg manifest mode
- `0003-layering.md` — Foundation layer, replacing the Core/Platform peer
  exception
- `0004-gl-loader.md` — GLEW over glad (core-profile configuration cost)
- `0005-custom-math-library.md` — hand-written math library over GLM
- `0006-test-framework.md` — doctest over Catch2/GoogleTest
- `0007-floating-point-equality.md` — no `operator==` on float-containing
  math types, `NearlyEqual` only
- `0008-api-stability-policy.md` — Stable/Deprecated/Internal API
  classification; `EngineMath` frozen at v1.0

## Status

Phase 1 (Engine Core / blank window at stable FPS) — done: real OpenGL
context via GLFW backend, vsync-locked main loop, Window/Input/Time
abstractions in place, only one backend (GLFW) exists so no plugin
machinery was built.

Phase 2 (Math library) — done, API-reviewed, and **frozen at v1.0**
(`docs/decisions/0008-api-stability-policy.md`): Vector2/3/4, Matrix3/4,
Quaternion, Transform, Ray, Plane, AABB, Sphere, and ray/AABB + ray/sphere
intersection tests. 33 unit tests / 93 assertions passing, zero compiler
warnings, no new dependencies beyond Foundation. All quality gate items
checked off except SIMD benchmarks (explicitly deferred pending a
profiling result that doesn't exist yet). Further changes to its public
API are bug/correctness/perf fixes only.

Phase 3 (Renderer) — design in progress: `docs/renderer/DESIGN.md`,
`API.md`, `ROADMAP.md`. No implementation yet, per the "interfaces before
OpenGL" rule.

## Changelog

- **0.4** — Declared the API stability policy (Stable/Deprecated/Internal,
  decision 0008) and froze `EngineMath` at v1.0 — no further signature
  changes without a new decision record. Began Phase 3 (Renderer) design:
  `docs/renderer/DESIGN.md`, `API.md`, `ROADMAP.md`. No renderer
  implementation yet.
- **0.3** — Phase 2 API review: removed exact `operator==` from
  `Vector2/3/4`/`Matrix4` in favor of `NearlyEqual` (decision 0007);
  expanded `Constants.h`; added fixed world-direction factories to
  `Vector3`; documented the column-vector matrix convention and `T*R*S`
  Transform order as permanent; audited and closed the implicit-conversion
  gap (`Vector4(Vector3, float)` is now `explicit`); documented the
  determinism limitation and the planned Math/Geometry split, both with
  explicit trigger conditions. Moved this file to
  `docs/architecture/ARCHITECTURE.md` and added the governance note
  restricting further edits to genuinely architectural decisions. Recorded
  the planned Renderer (Phase 3) sequence without starting implementation.
- **0.2** — Added the Foundation layer (was: Core/Platform peers with a
  Log/Assert exception). Added quality gates, roadmap, performance
  invariant, decision log entries 0005–0006. Narrowed Foundation's scope
  explicitly; deferred Reflection and ECS with stated trigger conditions.
  Math library (Phase 2) completed.
- **0.1** — Initial layering (Applications → Engine Modules → Core →
  Platform), "build only when two implementations exist" rule, first
  decision log entries (0001–0004), Phase 1 (Platform) completed.
