# SPDX-FileCopyrightText: 2021-2023  Daniel Aimé Valcour <fosssweeper@gmail.com>
#
# SPDX-License-Identifier: MIT

# Copyright (c) 2021-2023 Daniel Aimé Valcour
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

add_executable(rlh_example "")
add_executable(rlh::example ALIAS rlh_example)
add_subdirectory(src)
if (RLH_EXAMPLE_AUTO_FETCH)
    Include(FetchContent)
    FetchContent_Declare(
            glad
            GIT_REPOSITORY https://github.com/Dav1dde/glad
            GIT_TAG        v0.1.36
            FIND_PACKAGE_ARGS
    )
    FetchContent_Declare(
            stb
            GIT_REPOSITORY https://github.com/nothings/stb
    )
    FetchContent_Declare(
            gld
            GIT_REPOSITORY https://github.com/Journeyman-dev/gl_debug.h
            GIT_TAG        v1.0.0
    )
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw/
        GIT_TAG        3.3.8
        FIND_PACKAGE_ARGS
    )
    FetchContent_MakeAvailable(glad stb gld glfw)
endif()
target_link_libraries(rlh_example
    PUBLIC
        rlh::rlh
        glad
        gld::gld
        glfw
)
target_include_directories(rlh_example
    PUBLIC
        "${stb_SOURCE_DIR}"
)
# grab the content file paths
set(RLH_ASSET_FILES
    "cp_8x8_rgba_bg_alpha.png"
)
list(
    TRANSFORM RLH_ASSET_FILES
    PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/assets/"
)
add_custom_command(TARGET rlh_example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${RLH_ASSET_FILES}
        $<TARGET_FILE_DIR:rlh_example>
)