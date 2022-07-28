cmake_minimum_required(VERSION 3.19)

option(RLH_BUILD_EXAMPLES "Build the example projects" OFF)

PROJECT(roguelike.h
    VERSION 0.0
    DESCRIPTION "A header only roguelike rendering library."
    LANGUAGES C
)

set(RLH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/)

if (RLH_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()