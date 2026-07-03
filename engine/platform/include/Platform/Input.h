#pragma once

#include "Platform/Key.h"

namespace Engine {

class Window;

// Static facade over the active window's input state.
class Input {
public:
    static void Init(Window* window);
    static bool IsKeyPressed(Key key);
};

} // namespace Engine
