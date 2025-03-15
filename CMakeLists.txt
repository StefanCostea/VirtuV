cmake_minimum_required(VERSION 3.26)

project(VirtuV VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Options that modify the build system behaviour, if they are not applied correctly try rerunning the cmake with --fresh to ensure the CMakeCache is empty
option(ENABLE_TRACE "Enable trace logging in the project" OFF)

if(NOT DEFINED ENV{IS_ENV_SET})
message(FATAL_ERROR "Configuration Error: The required environment variable 'IS_ENV_SET' is not defined. Please ensure you have sourced the setup.env file before running CMake. For example, run:
    
    source setup.env

in your terminal, then re-run CMake to configure the project.")
endif()

#Supress find boost warning
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

#Add enable trace compile flag to enable traces in plt utils
if(ENABLE_TRACE)
    message(STATUS "Trace logging is enabled")
    add_compile_definitions(ENABLE_TRACE)
else()
    message(STATUS "Trace logging is disabled")
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
