# EngineFoundation

Zero-dependency infrastructure layer. Everything above Foundation depends on this; Foundation depends on nothing.

**Build type:** Static library (`EngineFoundation`)

## Contents

### Log

Printf-based logging with four levels:

```cpp
#include <Foundation/Log.h>

ENGINE_LOG_INFO("Window created: {}x{}", width, height);
ENGINE_LOG_WARN("Shader compilation failed: {}", infoLog);
ENGINE_LOG_ERROR("OpenGL context creation failed");
```

- `Trace` / `Info` / `Warn` -- stdout
- `Error` -- stderr
- All calls flush immediately (safe for crash logs)

### Assert

Debug assertion macro:

```cpp
#include <Foundation/Assert.h>

ENGINE_ASSERT(ptr != nullptr, "Pointer must be valid");
ENGINE_ASSERT(width > 0, "Width must be positive");
```

- Logs the condition and message on failure, then calls `std::abort()`
- Disabled in Release builds (`NDEBUG`)

## Design Rules

- **Zero dependencies** -- no libraries, no engine headers, nothing above this layer
- **No hidden allocation** -- stack-only, no heap usage
- **No platform-specific code** -- pure C++ standard library only
- **Deliberately narrow** -- only utilities that could appear in any C++ project

## Build

Built automatically as part of the full project build:

```bash
cmake --preset default
cmake --build --preset default
```
