cmake_minimum_required(VERSION 3.14)
project(assign21)
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
set(CMAKE_CXX_STANDARD 14)

add_executable(assign21 assign21.cxx)
target_link_libraries(assign21 ${VTK_LIBRARIES})
