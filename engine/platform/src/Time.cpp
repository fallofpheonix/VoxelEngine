#include "Platform/Time.h"

#include <chrono>

namespace Engine {

namespace {
using Clock = std::chrono::steady_clock;

Clock::time_point g_startTime = Clock::now();
Clock::time_point g_lastFrameTime = g_startTime;
float g_deltaTime = 0.0f;
} // namespace

void Time::Update() {
    const Clock::time_point now = Clock::now();
    g_deltaTime = std::chrono::duration<float>(now - g_lastFrameTime).count();
    g_lastFrameTime = now;
}

float Time::DeltaTime() {
    return g_deltaTime;
}

float Time::GetTime() {
    return std::chrono::duration<float>(Clock::now() - g_startTime).count();
}

} // namespace Engine
