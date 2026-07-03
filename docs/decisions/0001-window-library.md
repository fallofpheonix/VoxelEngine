# 0001 — Windowing/input library for Phase 1

## Problem

Phase 1 needs a window, an OpenGL context, and keyboard/mouse input, behind
a `Window`/`Input` interface so the backend is swappable later.

## Alternatives

- **GLFW** — small API scoped to window/context/input. Cross-platform.
  Widely used in graphics-from-scratch projects.
- **SDL2** — broader scope (audio, controllers, haptics, threads, timers).
  Useful later (Phase 13 audio, Phase 14 input) but more surface area than
  Phase 1 needs.
- **Native platform APIs** (Win32/Cocoa/X11 directly) — no dependency, but
  significantly more code before rendering a single triangle, and
  platform-specific from day one.

## Decision

GLFW, confined entirely to `engine/platform/src/GLFWWindow.cpp` and
`engine/platform/src/Input.cpp`. No other file includes `<GLFW/glfw3.h>`.

## Rationale

Matches Phase 1's actual scope (window, GL context, input, resize, main
loop). SDL2's extra scope is overhead we don't need yet. Native APIs teach
platform programming but cost more time than the phase warrants.

## Consequences

- `Window::Create()` / `Input::Init()` are the only engine-facing surface;
  swapping to SDL2 or a native backend later means writing a new backend
  file, not touching call sites.
- A second backend is not built now (see 0003 — build only when two
  implementations exist).
