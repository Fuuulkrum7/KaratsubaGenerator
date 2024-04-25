cmake_minimum_required(VERSION 3.21.0)

project(KaratsubaGenTests LANGUAGES CXX)
set(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} -std=c++17 -fPIC")
SET(GCC_COVERAGE_LINK_FLAGS "-lgcov")
set(CMAKE_CXX_STANDARD 17)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

include(FetchContent)

FetchContent_Declare(
  gtest

  # Url for googletest download.
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)

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

add_test(NAME ${PROJECT_NAME} COMMAND KaratsubaGen_test)
