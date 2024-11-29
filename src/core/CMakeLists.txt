# Recursively get all source files under core/
file(GLOB_RECURSE CORE_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_library(core ${CORE_SOURCES})

#Enable PIC -> for pybind
set_target_properties(core PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Define the Python bindings module
pybind11_add_module(virtuv_bindings ../bindings/virtuv_bindings.cpp)

# Link the bindings to the core library
target_link_libraries(virtuv_bindings PRIVATE core)

# Establish private include dirs
target_include_directories(core
        PUBLIC ${CMAKE_SOURCE_DIR}/src
)

#Set core as include dir root
include_directories(${CMAKE_SOURCE_DIR}/?)

# Compilation options
target_compile_options(core
        PRIVATE -Wall -Wextra -Wpedantic
)
