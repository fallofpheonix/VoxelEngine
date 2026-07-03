# EngineMath conventions

**Status: v1.0, Stable** (`docs/decisions/0008-api-stability-policy.md`).
Every type in this module's public headers is frozen — further changes are
bug fixes, correctness fixes, or performance improvements, not signature
changes or new behavior. New functionality is additive (new types/free
functions), not a modification of what's here.

These are the conventions every type in this module follows. Get them wrong
once and every downstream subsystem (Renderer, Physics, Animation) inherits
the bug — write code against these, don't re-derive them per call site.
These are permanent API commitments, not defaults to revisit per-feature —
changing any of them later means auditing every consumer.

- **Coordinate system**: right-handed. `Cross(x, y) == z`.
- **World-space directions are fixed**: `Vector3::Right()` (+X),
  `Left()` (-X), `Up()` (+Y), `Down()` (-Y), `Forward()` (-Z),
  `Back()` (+Z). `Forward = -Z` matches an OpenGL-style camera, which looks
  down -Z by default. Use these instead of inventing per-subsystem
  `(0,0,1)`-style constants — that's exactly the drift this fixes.
- **Matrix storage**: column-major (`m[col * N + row]`), matching OpenGL.
  `Matrix4::Data()` can be passed directly to `glUniformMatrix4fv` with
  `transpose = GL_FALSE`.
- **Matrix convention: column vectors.** Transform a vector with `M * v`,
  never `v * M` — there is no `operator*(Vector4, Matrix4)` and there
  should never be one. `A * B` applies `B` first, then `A`
  (`(A * B) * v == A * (B * v)`).
- **Transform composition order is fixed: `T * R * S`.**
  `Transform::ToMatrix4()` returns `Translation * Rotation * Scale`,
  meaning a point is scaled first, then rotated, then translated. This is
  the one true order for this engine — don't introduce a second Transform
  type with a different order.
- **Quaternion multiplication**: `a * b` rotates by `b` first, then `a`
  (same convention as matrices, for consistency).
- **Angles**: radians everywhere in the API. `DegToRad`/`RadToDeg` in
  `Constants.h` convert at the boundary (e.g. reading a level-editor value
  authored in degrees).
- **Projection depth range**: OpenGL NDC, `z` in `[-1, 1]`. If a Vulkan
  backend is ever added (see `docs/decisions/0003-layering.md`'s
  "two implementations" rule — not yet), it needs its own
  `Perspective`/`Orthographic` variants for `[0, 1]` depth, not a
  modification of these.
- **Normalization is not automatic**: `Vector3`/`Quaternion` don't enforce
  unit length on construction. Call `.Normalized()` explicitly. `Ray` and
  `Plane` document which fields the caller is expected to keep normalized.
- **No `operator==` on float-containing types.** `Vector2/3/4` and
  `Matrix4` deliberately expose no exact equality operator — `0.1f + 0.2f
  != 0.3f` makes exact comparison a footgun for anything computed. Use
  `.NearlyEqual(other, epsilon = kEpsilon)` instead. See
  `docs/decisions/0007-floating-point-equality.md`.
- **No implicit conversions.** Every constructor in this module requires
  two or more arguments except copy/move, so `Vector3 v = 5;`-style
  accidental conversions can't happen. The one constructor that converts
  between types (`Vector4(const Vector3&, float)`) is marked `explicit`.
  If a future addition needs a single-argument constructor (e.g. a
  broadcast `Vector3(float)`), mark it `explicit` too — don't assume this
  rule stays true by accident.
- **No hidden allocation**: every type here is a plain value type on the
  stack. None of this module allocates; keep it that way so it's always
  safe to use inside a render/physics/animation tight loop.
- **Determinism is not yet guaranteed across platforms.** `std::sin`,
  `std::cos`, `std::sqrt`, etc. are not bit-identical across
  compilers/platforms/optimization levels. That's fine for rendering and
  local simulation, but it means this library is *not* currently suitable
  for lockstep/rollback networking, which needs identical results on every
  peer given identical input. Revisit (likely with a fixed-point or
  explicitly-specified-algorithm math path) only when the Networking phase
  actually needs it — see `docs/architecture/ARCHITECTURE.md`'s roadmap.
  Building deterministic math now, with no consumer, would be exactly the
  kind of speculative work this project avoids.

## Geometry types will eventually split out

`Ray`, `Plane`, `AABB`, `Sphere`, and `Intersections.h` live in this module
today because there are only five of them. Once Physics/Renderer need
`OBB`, `Frustum`, `Triangle`, or `Capsule`, this set is big enough to
justify its own `engine/geometry` module (mirroring how `engine/foundation`
split out of `engine/core`, see `docs/decisions/0003-layering.md`). Not
done now — five small header-only files don't yet cause the problem a
split would solve.
