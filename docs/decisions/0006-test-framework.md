# 0006 — Test framework

## Problem

Phase 2's quality gate requires unit tests. Need a C++ test framework
available via vcpkg.

## Alternatives

- **Catch2** — full-featured, widely used, slower to compile (single
  translation unit with the full framework, or precompiled library mode).
- **doctest** — single header, near-zero compile-time overhead, subset of
  Catch2's macro syntax (`TEST_CASE`, `CHECK`, `doctest::Approx`).
- **GoogleTest** — most common in large C++ codebases, heavier setup
  (separate main, `EXPECT_`/`ASSERT_` macro split), more machinery than a
  first test suite needs.

## Decision

doctest.

## Rationale

The math library alone will accumulate tests fast as later phases add
dependents (Renderer, Physics, Animation all consume it). doctest's
near-zero compile overhead keeps the test binary fast to rebuild during
iteration, and its API is close enough to Catch2/GoogleTest that switching
later (if a feature doctest lacks becomes necessary) is a low-cost rewrite,
not a redesign.

## Consequences

- `tests/math/` is the first test target (`MathTests`), wired into CTest
  via `add_test`.
- Each future module with real logic (not just interface declarations)
  should get a matching `tests/<module>/` target following the same
  pattern.
