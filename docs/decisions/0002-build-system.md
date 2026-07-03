# 0002 — Build system and dependency management

## Problem

A multi-year, multi-dependency C++ project needs reproducible builds across
machines and, eventually, platforms.

## Alternatives

- **CMake + vcpkg (manifest mode)** — cross-platform, dependency versions
  pinned in `vcpkg.json`, standard for modern C++ engine projects.
- **CMake with vendored/submodule dependencies** — reproducible, no
  package-manager learning curve, manual update burden.
- **Plain Makefile / single-file compiler invocation** (prior `.vscode/tasks.json`
  setup) — zero tooling, doesn't scale past one source file or one
  dependency.

## Decision

CMake with `CMakePresets.json`, vcpkg in manifest mode via `vcpkg.json`,
vcpkg itself vendored at `third_party/vcpkg`. One CMake target per module.

## Rationale

The 35-phase roadmap implies dozens of third-party libraries over time
(physics, audio, scripting, image/model import). vcpkg manifest mode keeps
versions declared in one file and avoids manual dependency management at
that scale. Per-module CMake targets keep builds incremental and make the
layering rules in `docs/architecture/ARCHITECTURE.md` enforceable (a module can only link
what it explicitly depends on).

## Consequences

- New dependencies are added to `vcpkg.json`, not installed ad hoc.
- First configure is slow (vcpkg builds dependencies from source); later
  configures hit vcpkg's binary cache.
- Every new module needs its own `CMakeLists.txt` and an `add_subdirectory`
  entry in the root file.
