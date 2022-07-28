cmake_minimum_required(VERSION 3.19)

option(RLH_BUILD_EXAMPLES "Build the example projects" OFF)

PROJECT(rlh
    VERSION 1.1
    DESCRIPTION "A header only roguelike rendering library."
    LANGUAGES C
)

add_library(${PROJECT_NAME} INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/include/roguelike.h"
)

target_include_directories(${PROJECT_NAME}
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/include/"
)

if (RLH_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
