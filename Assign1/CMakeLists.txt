cmake_minimum_required(VERSION 3.14)
project(graphDrawing)
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
set(CMAKE_CXX_STANDARD 14)

add_executable(graphDrawing graphDrawing.cxx)
target_link_libraries(graphDrawing ${VTK_LIBRARIES})