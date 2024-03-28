add_library(
  Graph 
  SHARED 
  BasicType.h
  BasicType.cpp
  GraphVertex.h
  GraphVertex.cpp
  OrientedGraph.h
  OrientedGraph.cpp
)

target_include_directories(Graph PUBLIC ${PROJECT_SOURCE_DIR})
