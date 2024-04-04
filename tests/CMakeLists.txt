cmake_minimum_required(VERSION 3.21.0)

project(KaratsubaGenTests LANGUAGES CXX)
set(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} -std=c++17 -fPIC")
SET(GCC_COVERAGE_LINK_FLAGS "-lgcov")
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
  gtest

  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(gtest)

set(SOURCES_TEST
  graph/GraphVertexTests.cpp
  graph/OrientedGraphTests.cpp
)

enable_testing()
add_executable(${PROJECT_NAME} ${SOURCES_TEST})

target_link_libraries(${PROJECT_NAME}
  Graph
  Karatsuba
  gtest
  gtest_main
)

# add_test(NAME ${PROJECT_NAME} COMMAND KaratsubaGen_test)