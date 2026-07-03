add_library(EngineFoundation STATIC
    src/Log.cpp
)

target_include_directories(EngineFoundation PUBLIC include)
target_compile_features(EngineFoundation PUBLIC cxx_std_20)
