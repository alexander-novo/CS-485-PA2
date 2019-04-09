cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_COMPILER    "gcc")
set(CMAKE_CXX_COMPILER  "g++")
set(CMAKE_BUILD_TYPE Debug)

project( toonify )
find_package( OpenCV REQUIRED )
add_executable( toonify src/toon.cpp )
target_link_libraries( toonify ${OpenCV_LIBS} )

project( compress )
add_executable( compress src/compress.cpp )
target_link_libraries( compress ${OpenCV_LIBS} )

project( trees )
add_executable( trees src/trees.cpp )
target_link_libraries( trees ${OpenCV_LIBS} )
