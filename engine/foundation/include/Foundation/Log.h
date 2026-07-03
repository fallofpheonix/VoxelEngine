#pragma once

#include <string>

namespace Engine {

enum class LogLevel { Trace, Info, Warn, Error };

void Log(LogLevel level, const std::string& message);

} // namespace Engine

#define ENGINE_LOG_INFO(msg) ::Engine::Log(::Engine::LogLevel::Info, msg)
#define ENGINE_LOG_WARN(msg) ::Engine::Log(::Engine::LogLevel::Warn, msg)
#define ENGINE_LOG_ERROR(msg) ::Engine::Log(::Engine::LogLevel::Error, msg)
