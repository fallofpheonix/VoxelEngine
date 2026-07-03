find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)

add_library(EnginePlatform STATIC
    src/GLFWWindow.cpp
    src/Input.cpp
    src/Time.cpp
)

target_include_directories(EnginePlatform PUBLIC include)
target_link_libraries(EnginePlatform PUBLIC EngineFoundation glfw GLEW::GLEW)
