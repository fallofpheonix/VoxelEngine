# 0008 — API stability policy; freeze Math at v1.0

## Problem

Once Renderer, Physics, and Animation start depending on `EngineMath`,
changing its function signatures becomes expensive across every dependent.
There was no stated policy for which parts of a module's API are safe to
keep depending on versus which are internal details that can still change.

## Decision

Every module's public surface is classified as one of:

```text
Stable      — API-reviewed, frozen except bug/correctness/perf fixes.
              Signature changes require a new decision record.
Deprecated  — still present, has a Stable replacement, scheduled for removal.
Internal    — implementation detail. Not included in the module's public
              headers' documented surface, even if technically reachable
              (e.g. a backend .cpp's file-local helpers). No stability
              guarantee at all.
```

`EngineMath` is declared **v1.0, Stable**, effective this commit:

```text
Vector2, Vector3, Vector4     Stable
Matrix3, Matrix4              Stable
Quaternion                    Stable
Transform                     Stable
Ray, Plane, AABB, Sphere      Stable
Intersections.h free functions Stable
Constants.h (kPi, kEpsilon, DegToRad, Lerp, Clamp, NearlyEqual, ...) Stable
```

No internal-only helpers exist in `engine/math` today — everything in its
public headers is part of the reviewed, Stable surface.

## Rationale

Phase 2's API review (see `docs/decisions/0007-floating-point-equality.md`
and `docs/architecture/ARCHITECTURE.md`'s "Phase 2 API review" table)
already exercised every type against real usage (33 tests, 93 assertions).
Freezing now, before Renderer/Physics/Animation start writing code against
it, is the cheapest point to freeze at — every dependent added after this
point raises the cost of a future signature change.

## Consequences

- Further changes to `engine/math` public headers are bug fixes,
  correctness fixes, or performance improvements only — not new features,
  not signature changes, until a concrete need forces a `v2.0` (which
  would get its own decision record).
- New math functionality (e.g. `OBB`, `Frustum`, SIMD paths) is additive —
  new types/functions, not changes to existing ones — until proven to
  need a breaking change.
- Every future module (Renderer, Physics, ...) should state its own
  Stable/Deprecated/Internal breakdown once its API review happens, not
  before — freezing Renderer today would be premature (see
  `docs/renderer/DESIGN.md`, which explicitly hasn't shipped an
  implementation yet).
