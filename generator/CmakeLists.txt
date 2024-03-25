add_library(
  Karatsuba 
  SHARED 
  KaratsubaGen.h
  KaratsubaGen.cpp
)

target_include_directories(Karatsuba PUBLIC ${PROJECT_SOURCE_DIR})
