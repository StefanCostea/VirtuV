cmake_minimum_required(VERSION 3.26)

project(VirtuV VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Supress find boost warning
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

# Find Python interpreter and Pybind11
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 REQUIRED)

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
target_link_libraries(virtuv PRIVATE
        core
        ${Boost_LIBRARIES}
)

enable_testing()

find_package(Boost REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})
