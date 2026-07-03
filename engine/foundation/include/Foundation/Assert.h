#pragma once

#include "Foundation/Log.h"
#include <cstdlib>

#ifdef NDEBUG
#define ENGINE_ASSERT(condition, message) ((void)0)
#else
#define ENGINE_ASSERT(condition, message)                                    \
    do {                                                                     \
        if (!(condition)) {                                                  \
            ::Engine::Log(::Engine::LogLevel::Error,                         \
                           std::string("Assertion failed: ") + (message));   \
            std::abort();                                                    \
        }                                                                    \
    } while (0)
#endif
