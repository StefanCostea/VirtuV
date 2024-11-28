cmake_minimum_required(VERSION 3.26)

project(VirtuV VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src/core)
add_subdirectory(src/utils)
add_subdirectory(tests)

add_executable(virtuv
        src/main.cpp
)

# Public API
target_include_directories(virtuv
        PUBLIC ${CMAKE_SOURCE_DIR}/include
)

# Link core to executable
target_link_libraries(virtuv
        PRIVATE core
)

enable_testing()
