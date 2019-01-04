cmake_minimum_required( VERSION 2.8 )

project(HDR)

set( CMAKE_BUILD_TYPE Release )
set( CMAKE_CXX_FLAGS "-std=c++11 -O3" )

#opencv
find_package( OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS})

add_executable( HDR HDR.cpp)
target_link_libraries( HDR ${OpenCV_LIBS})