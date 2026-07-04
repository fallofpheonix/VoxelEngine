# VoxelEngine

A modular, data-oriented engine platform built in C++20. Core stays small, every subsystem is replaceable, and the same architecture powers games, simulations, robotics, visualization, and voxel worlds.

**Status:** Phase 1 (Platform) and Phase 2 (Math) complete. Phase 3 (Renderer) in design.

## Architecture

```
Applications
      |
      v
Engine Modules   (Renderer, Physics, Audio, Scene, Animation, Networking, Scripting)
      |
      v
Core / Platform  (Math, Memory, Jobs, Containers, Events / Window, Input, Filesystem, Time)
      |
      v
Foundation       (Log, Assert, Types, UUID, Hash, Config)
```

**Single invariant:** Dependencies always point downward. Lower layers never depend on higher layers. No exceptions.

See [docs/architecture/ARCHITECTURE.md](docs/architecture/ARCHITECTURE.md) for the full design document.

## Modules

| Module | Status | Description |
|--------|--------|-------------|
| `engine/foundation` | Done | Log and Assert. Zero dependencies. |
| `engine/math` | Done (frozen v1.0) | Header-only math library. Vectors, matrices, quaternions, transforms, geometry. 33 tests, 93 assertions. |
| `engine/platform` | Done | Window, Input, Time abstractions. GLFW/OpenGL 3.3 backend. |
| `engine/renderer` | Design only | IRenderer interface and 13-step build plan. No implementation yet. |
| `engine/physics` | Planned | Phase 7 |
| `engine/audio` | Planned | Phase 9 |
| `engine/animation` | Planned | Phase 8 |
| `engine/networking` | Planned | Phase 10 |
| `engine/scripting` | Planned | Phase 11 |

## Prerequisites

- **C++20** compiler (Clang 14+, GCC 11+, MSVC 2022+)
- **CMake** 3.24+
- **Ninja** build system
- **vcpkg** (vendored at `third_party/vcpkg`)
- **OpenGL 3.3** capable GPU

## Build

```bash
# Clone with vcpkg submodule
git clone https://github.com/fallofpheonix/VoxelEngine.git
cd VoxelEngine
git submodule update --init --recursive

# Bootstrap vcpkg (if needed)
./third_party/vcpkg/bootstrap-vcpkg.sh

# Configure and build
cmake --preset default
cmake --build --preset default
```

First configure builds dependencies from source via vcpkg. Subsequent runs use the binary cache.

## Run

```bash
# Sandbox application (1280x720 window, color-cycling background)
./build/default/sandbox/Sandbox

# Press Escape to close
```

## Tests

```bash
# Run all tests
ctest --test-dir build/default

# Run math tests directly
./build/default/tests/math/MathTests

# Filter by test case
./build/default/tests/math/MathTests --test-case="Vector3*"
```

**Test coverage:** 33 test cases, 93 assertions across vectors, matrices, quaternions, transforms, and geometry primitives.

## Project Structure

```
VoxelEngine/
├── engine/                  # Engine libraries
│   ├── foundation/          # Log, Assert (zero dependencies)
│   ├── math/                # Header-only math library
│   ├── platform/            # Window/Input/Time (GLFW backend)
│   ├── renderer/            # [empty] Phase 3
│   ├── physics/             # [empty] Phase 7
│   ├── audio/               # [empty] Phase 9
│   ├── animation/           # [empty] Phase 8
│   ├── ecs/                 # [empty] deferred
│   ├── networking/          # [empty] Phase 10
│   └── scripting/           # [empty] Phase 11
├── sandbox/                 # Demo application
├── tests/                   # Unit tests (doctest)
├── docs/                    # Architecture docs, ADRs, renderer design
├── assets/                  # [empty] reserved
├── editor/                  # [empty] planned late
├── tools/                   # [empty] reserved
└── third_party/vcpkg/       # Package manager (gitignored)
```

## Dependencies

Managed via [vcpkg](https://github.com/microsoft/vcpkg) in manifest mode:

| Library | Purpose |
|---------|---------|
| [GLFW 3](https://www.glfw.org/) | Window/context/input |
| [GLEW](http://glew.sourceforge.net/) | OpenGL function loader |
| [doctest](https://github.com/doctest/doctest) | Unit test framework |

## Documentation

- [Architecture](docs/architecture/ARCHITECTURE.md) -- Full engine design document
- [Math Conventions](engine/math/README.md) -- Coordinate system, matrix storage, transform order
- [Renderer Design](docs/renderer/DESIGN.md) -- Renderer architecture
- [Renderer API](docs/renderer/API.md) -- Proposed IRenderer interface
- [Renderer Roadmap](docs/renderer/ROADMAP.md) -- 13-step build sequence

### Architecture Decision Records

| ADR | Decision |
|-----|----------|
| [0001](docs/decisions/0001-window-library.md) | GLFW over SDL2/native |
| [0002](docs/decisions/0002-build-system.md) | CMake + vcpkg manifest mode |
| [0003](docs/decisions/0003-layering.md) | Foundation layer model |
| [0004](docs/decisions/0004-gl-loader.md) | GLEW over glad |
| [0005](docs/decisions/0005-custom-math-library.md) | Custom math over GLM |
| [0006](docs/decisions/0006-test-framework.md) | doctest over Catch2/GoogleTest |
| [0007](docs/decisions/0007-floating-point-equality.md) | NearlyEqual only, no operator== |
| [0008](docs/decisions/0008-api-stability-policy.md) | Stable/Deprecated/Internal API classification |

## License

No license specified yet.
