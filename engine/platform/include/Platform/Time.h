#pragma once

namespace Engine {

// Static facade over frame timing. Backend-independent (std::chrono based),
// so it works the same regardless of which windowing backend is active.
class Time {
public:
    static void Update();
    static float DeltaTime();
    static float GetTime();
};

} // namespace Engine
