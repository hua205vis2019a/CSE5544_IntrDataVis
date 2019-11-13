cmake_minimum_required(VERSION 3.3 FATAL_ERROR)


project (Isosurface5544)
find_package(VTK REQUIRED)

if (NOT VTK_FOUND)
    message("Skipping Isosurface5544: ${VTK_NOT_FOUND_MESSAGE}")
    return ()
endif()
message (STATUS "VTK_VERSION: ${VTK_VERSION}")
if (VTK_VERSION VERSION_LESS "8.90.0")
    # old system
    include(${VTK_USE_FILE})
    add_executable(Isosurface5544 MACOSX_BUNDLE Isosurface5544.cxx )
    target_link_libraries(Isosurface5544 PRIVATE ${VTK_LIBRARIES})
else ()
    # include all components
    add_executable(Isosurface5544 MACOSX_BUNDLE Isosurface5544.cxx )
    target_link_libraries(Isosurface5544 PRIVATE ${VTK_LIBRARIES})
    # vtk_module_autoinit is needed
    vtk_module_autoinit(
            TARGETS Isosurface5544
            MODULES ${VTK_LIBRARIES}
    )
endif ()
