# SPDX-FileCopyrightText: 2021-2022  Daniel Aimé Valcour <fosssweeper@gmail.com>
#
# SPDX-License-Identifier: MIT

# Copyright (c) 2021-2022 Daniel Aimé Valcour
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

# create the executable for the example
set(RLH_EXAMPLE_SOURCE_FILES
    "gld_impl.c"
    "main.c"
	"rlh_impl.c"
	"stb_image_impl.c"
)
list(
    TRANSFORM RLH_EXAMPLE_SOURCE_FILES
    PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/src/"
)
source_group("sources"
    FILES ${RLH_EXAMPLE_SOURCE_FILES}
)
add_executable(rlh_example
    ${RLH_EXAMPLE_SOURCE_FILES}
)
target_link_libraries(rlh_example
    PRIVATE
        glfw
        glad
        stb
        rlh
        gld
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
