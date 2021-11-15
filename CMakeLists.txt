cmake_minimum_required(VERSION 3.19)
	
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(RLH_STANDALONE TRUE)
	set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")
endif()

set(RLH_CMAKE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

option(RLH_BUILD_EXAMPLES "Build the example projects" OFF)
option(RLH_NO_RAYLIB_EXAMPLE "Do not build the raylib example if building examples." ON)

PROJECT(roguelike.h
	VERSION 0.0
	DESCRIPTION "A header only roguelike rendering library."
	LANGUAGES C
)

set(RLH_INCLUDE_DIR ${RLH_CMAKE_SOURCE_DIR}/include/)

if (RLH_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()