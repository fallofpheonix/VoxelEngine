# 0005 — Custom math library instead of GLM

## Problem

Phase 2 needs Vector2/3/4, Matrix3/4, Quaternion, Transform, Ray, Plane,
AABB, Sphere, and intersection tests.

## Alternatives

- **GLM** — header-only, GLSL-styled, the de facto default for
  from-scratch OpenGL projects. Was already in `vcpkg.json` from initial
  setup, unused.
- **Custom library** (`engine/math`) — full control over conventions
  (column-major storage matching our GL usage, explicit `ENGINE_ASSERT` on
  invalid operations like normalizing a zero vector or inverting a
  singular matrix), and it's genuinely part of what this project is for.

## Decision

Custom library. Removed the unused `glm` entry from `vcpkg.json`.

## Rationale

This project's stated goal is learning/building the engine's own
foundation, not assembling one from off-the-shelf parts where avoidable.
A hand-written math library is also small enough (13 headers) to fully
own and matches the "own the boundary you actually need to own" spirit of
`docs/decisions/0003-layering.md`.

## Consequences

- All conventions (column-major, right-handed, radians, OpenGL `[-1,1]`
  NDC depth) are recorded in `engine/math/README.md` since there's no
  external spec (like GLM's docs) to defer to.
- A Vulkan renderer, if ever built, needs its own `[0,1]`-depth projection
  variants — GLM ships both; we don't yet, because nothing needs it.
