#pragma once

namespace Engine {

// Backend-agnostic key codes. Only the GLFW backend .cpp knows how these
// map to GLFW_KEY_*.
enum class Key {
    W, A, S, D,
    Up, Down, Left, Right,
    Space, Escape,
};

} // namespace Engine
