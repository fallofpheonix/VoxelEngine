# Sandbox

Demo application showcasing Phase 1 (Platform) and Phase 2 (Math) of the engine.

**Build type:** Executable (`Sandbox`)

**Dependencies:** EngineFoundation, EngineMath, EnginePlatform

## What It Does

- Opens a 1280x720 window titled "Voxel Engine - Sandbox"
- Initializes the Input system
- Runs a main loop: poll events, update time, render a sinusoidal color-cycling background
- Displays FPS in the window title
- Closes on Escape or window close

This is a Phase 1 deliverable -- a window running at stable FPS via the platform abstractions, with raw `glClearColor`/`glClear` as a placeholder until the Renderer phase.

## Build & Run

```bash
cmake --preset default
cmake --build --preset default
./build/default/sandbox/Sandbox
```

Press **Escape** to close.
