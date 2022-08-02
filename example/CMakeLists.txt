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
