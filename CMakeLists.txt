# SPDX-FileCopyrightText: 2021-2022 Daniel Valcour <fossweeper@gmail.com>
#
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.19)

PROJECT(rlh
    VERSION 1.4
    DESCRIPTION "A header only roguelike rendering library."
    LANGUAGES C
)

option(RLH_BUILD_EXAMPLE "Build the example project" OFF)

add_library(${PROJECT_NAME} INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/include/roguelike.h"
)

target_include_directories(${PROJECT_NAME}
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/include/"
)

if (RLH_BUILD_EXAMPLE)
    add_subdirectory(example)
endif()
