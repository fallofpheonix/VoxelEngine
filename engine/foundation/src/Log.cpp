#include "Foundation/Log.h"

#include <cstdio>

namespace Engine {

namespace {
const char* LevelTag(LogLevel level) {
    switch (level) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Info:  return "INFO";
        case LogLevel::Warn:  return "WARN";
        case LogLevel::Error: return "ERROR";
    }
    return "UNKNOWN";
}
} // namespace

void Log(LogLevel level, const std::string& message) {
    FILE* stream = level == LogLevel::Error ? stderr : stdout;
    std::fprintf(stream, "[%s] %s\n", LevelTag(level), message.c_str());
    std::fflush(stream);
}

} // namespace Engine
