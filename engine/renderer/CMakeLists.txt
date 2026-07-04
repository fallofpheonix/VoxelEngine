find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)

add_library(EngineRenderer STATIC
    src/OpenGLRenderer.cpp
)

target_include_directories(EngineRenderer PUBLIC include)
target_link_libraries(EngineRenderer PUBLIC EngineFoundation EngineMath EnginePlatform glfw GLEW::GLEW)
