# 0007 — No exact `operator==` on float-containing math types

## Problem

`Vector2/3/4` and `Matrix4` initially exposed `operator==`/`operator!=`
doing exact, bitwise-equivalent float comparison.

## Alternatives

- **Keep exact `operator==`** — matches built-in float semantics, useful
  for exact-zero checks or as a map/set key, but invites bugs the moment
  it's used on anything computed: `0.1f + 0.2f != 0.3f` is standard IEEE
  754 behavior, not a bug, and exact equality on a rotated/transformed
  vector will intermittently fail for reasons that look like a math bug
  but aren't.
- **`NearlyEqual(other, epsilon)` only, no `operator==`** — removes the
  footgun by removing the API that invites it. Callers who genuinely need
  exact float comparison (rare) can compare `.x`/`.y`/`.z` directly.

## Decision

No `operator==` on `Vector2`, `Vector3`, `Vector4`, or `Matrix4`. Each
exposes `NearlyEqual(other, epsilon = kEpsilon)` instead, where `kEpsilon`
(`1e-5f`) is defined once in `Constants.h`.

## Rationale

A math library used by nearly every future subsystem should make the
correct comparison the only comparison available, not the opt-in one.
Deleting `operator==` rather than just adding `NearlyEqual` alongside it
means a test or gameplay comparison can't accidentally use the wrong one.

## Consequences

- All comparisons in `tests/math/*.cpp` use `.NearlyEqual(...)`, not `==`.
- `Quaternion` never had `operator==` and doesn't get one either, for the
  same reason.
- If a genuine need for exact equality shows up later (e.g. a hash-based
  cache keyed on vectors), it should compare fields directly at the call
  site, not reintroduce a library-wide `operator==`.
