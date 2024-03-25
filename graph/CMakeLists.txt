add_library(
  Graph 
  SHARED 
  GraphVertex.h
  GraphVertex.cpp
  OrientedGraph.h
  OrientedGraph.cpp
)

target_include_directories(Graph PUBLIC ${PROJECT_SOURCE_DIR})
