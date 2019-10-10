cmake_minimum_required(VERSION 3.14)
project(assign3)
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})


set(CMAKE_CXX_STANDARD 14)

add_executable(assign3 main.cpp)
target_link_libraries(assign3 ${VTK_LIBRARIES})
