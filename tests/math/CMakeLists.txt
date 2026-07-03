find_package(doctest CONFIG REQUIRED)

add_executable(MathTests
    main.cpp
    VectorTests.cpp
    MatrixTests.cpp
    QuaternionTests.cpp
    TransformTests.cpp
    GeometryTests.cpp
)

target_link_libraries(MathTests PRIVATE EngineMath doctest::doctest)

include(CTest)
add_test(NAME MathTests COMMAND MathTests)
