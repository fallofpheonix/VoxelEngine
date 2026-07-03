# 0003 — Layer model: Foundation / Platform / Core / Engine / Applications

## Problem

Define a dependency direction strict enough to survive a multi-year,
multi-contributor engine without drifting into circular or upward
dependencies.

## Alternatives considered

1. **Applications → Engine Modules → Core → Platform**, Platform as the
   lowest layer. Rejected: `Log`/`Assert` are used by Platform code
   (`GLFWWindow.cpp`, `Input.cpp`) but conceptually belong to Core, which
   would put Platform *above* Core, not below — contradicting itself.
2. **Core and Platform as peers**, with a documented exception letting
   Platform use Core's zero-dependency `Log`/`Assert`. Worked, but required
   special-casing "Core except logging," which is exactly the kind of
   exception that erodes a dependency rule over time.
3. **Foundation as a fourth, strictly-lower layer** holding only
   zero-dependency infrastructure (`Log`, `Assert`, and future `Types`,
   `Result`, `UUID`, `Hash`, `Config`). Core and Platform both depend on
   Foundation only, never on each other.

## Decision

Option 3.

```text
Applications
      │
      ▼
Engine Modules   (Renderer, Physics, Audio, Scene, Animation, Networking, Resources, Scripting)
      │
      ▼
Core             (Math, ECS, Memory, Jobs, Containers, Reflection, Serialization, Events)
      │             Platform (Window, Input, Filesystem, Time, OS Services)
      ▼             │
      └──────┬───────┘
             ▼
        Foundation  (Log, Assert, Types, Result, UUID, Hash, Config)
```

Rule, with no exceptions: Foundation depends on nothing. Core depends only
on Foundation. Platform depends only on Foundation. Engine Modules depend
on Core and/or Platform. Applications depend on Engine Modules.

## Rationale

Removes the special case entirely. `EngineCore` and `EnginePlatform` are
now genuinely independent of each other — either can be modified, tested,
or replaced without touching the other — which is the actual property the
layering exists to protect.

## Consequences

- `engine/foundation/` holds `Log`/`Assert` (moved out of the now-removed
  `engine/core/`).
- `EnginePlatform` links `EngineFoundation`, not `EngineCore`.
- When Core gains real content (Math, ECS, Jobs, ...) in Phase 2+, it must
  not reach into Platform, and vice versa. Enforced by review for now (see
  0003-followup on architectural metrics/CI enforcement, not yet built —
  deferred per the "build only when there's a concrete problem" rule).
