set(EXAMPLE_NAME "RLH-Example")
# create the executable for the example
add_executable(${EXAMPLE_NAME}
    ${EXAMPLE_NAME}/main.c
)
find_package(glfw3 CONFIG REQUIRED)
find_path(STB_INCLUDE_DIRS "stb.h")
find_package(glad CONFIG REQUIRED)
target_link_libraries(${EXAMPLE_NAME}
    PRIVATE
        glfw
        glad::glad
)
find_path(STB_INCLUDE_DIRS "stb_c_lexer.h")
target_include_directories(${EXAMPLE_NAME} PRIVATE
    ${RLH_INCLUDE_DIR}
    ${STB_INCLUDE_DIRS}
)
set_property(TARGET ${EXAMPLE_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${EXAMPLE_NAME}>)
# grab the content file paths
file(GLOB asset_files
    assets/*
)