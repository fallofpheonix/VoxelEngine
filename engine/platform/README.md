# EnginePlatform

OS/hardware adapter layer. Provides window management, input handling, and timing through an abstract interface with a GLFW/OpenGL backend.

**Build type:** Static library (`EnginePlatform`)

**Dependencies:** EngineFoundation, GLFW, GLEW

## Contents

### Window

Abstract window interface with backend-agnostic API:

```cpp
#include <Platform/Window.h>

WindowProps props;
props.width = 1280;
props.height = 720;
props.title = "My Application";

auto window = Window::Create(props);

while (!window->ShouldClose()) {
    window->PollEvents();
    // ... render ...
    window->SwapBuffers();
}
```

**Backend:** `GLFWWindowImpl` -- the only file that touches GLFW. Creates OpenGL 3.3 Core Profile context, handles framebuffer resize, enables vsync.

### Input

Static facade for keyboard input:

```cpp
#include <Platform/Input.h>

Input::Init(window);

if (Input::IsKeyPressed(Key::Escape)) {
    // exit
}

if (Input::IsKeyPressed(Key::W)) {
    // move forward
}
```

**Supported keys:** W, A, S, D, arrow keys, Space, Escape.

### Time

Static facade for frame timing:

```cpp
#include <Platform/Time.h>

while (!window->ShouldClose()) {
    Time::Update();
    float dt = Time::DeltaTime();
    float elapsed = Time::GetTime();
    // ... update with dt ...
}
```

Uses `std::chrono::steady_clock` -- backend-independent.

## Design Rules

- **Interface splits from implementation** -- `Window.h` is the abstract interface, `GLFWWindow.cpp` is the concrete backend
- **Platform code never leaks** -- no GLFW/GL code appears outside `GLFWWindow.cpp`
- **Backend-agnostic** -- `Key.h` and `Input`/`Time` facades don't reference GLFW
- **Reference-counted init** -- GLFW is initialized once and terminated when the last window closes

## Build

Built automatically as part of the full project build:

```bash
cmake --preset default
cmake --build --preset default
```
