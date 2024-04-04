cmake_minimum_required(VERSION 3.13)

project(
    KaratsubaGen
    VERSION 0.1.0
    LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(MAIN_TARGET karatsuba)

option(BUILD_SHARED_PROJECT_LIBS "Build using shared libraries" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} -std=c++17 -fPIC")

add_subdirectory(graph)
add_subdirectory(generator)

add_subdirectory(tests)

set(
    PROJECT_LIBS

    # Internal libraries
    Graph
    Karatsuba
)

set(SOURCES

    # All sources used
    graph/GraphVertex.cpp
    graph/OrientedGraph.cpp

    generator/KaratsubaGen.cpp

    main.cpp
)

add_executable(${MAIN_TARGET} ${SOURCES})
target_link_libraries(${MAIN_TARGET} ${PROJECT_LIBS})
