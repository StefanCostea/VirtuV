# Recursively get all source files under core/
file(GLOB_RECURSE CORE_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_library(core ${CORE_SOURCES})

# Establish private include dirs
target_include_directories(core
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}  # Local headers for modules
)

# Compilation options
target_compile_options(core
        PRIVATE -Wall -Wextra -Wpedantic
)
