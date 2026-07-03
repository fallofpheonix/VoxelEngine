# 0004 — OpenGL function loader

## Problem

Phase 1's window needs an OpenGL 3.3 core profile context and a loader for
its function pointers.

## Alternatives

- **glad**, via vcpkg's `glad` port — the port defaults to the
  *compatibility* profile; getting a core-profile loader requires either a
  custom triplet or an overlay port (per the port's own build message).
  That's extra build-system machinery for what should be a one-line
  dependency.
- **GLEW**, via vcpkg's `glew` port — ships a `GLEW::GLEW` CMake target
  directly usable with a core profile context (with the standard
  `glewExperimental = GL_TRUE` workaround before `glewInit()`).

## Decision

GLEW.

## Rationale

Avoids configuring a custom vcpkg triplet or overlay port just to get a
core-profile loader. `GLEW::GLEW` worked with zero extra configuration
against a `GLFW_OPENGL_CORE_PROFILE` context.

## Consequences

- `engine/platform/src/GLFWWindow.cpp` sets `glewExperimental = GL_TRUE`
  before `glewInit()` — required for core-profile contexts on some
  drivers, harmless where it isn't needed.
- If a future renderer needs bindless resources or other modern-GL/Vulkan
  features where GLEW's maintenance status becomes a liability, revisit
  (glad with a proper overlay port, or glbinding) — not a concern for
  Phase 1's triangle/cube-level rendering.
