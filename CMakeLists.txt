cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(JAUT VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(JAUT_BUILD_TESTS "Build unit tests for the JAUT bundle" OFF)

add_subdirectory(datasets)
include_directories("datasets/include")

if (JAUT_BUILD_TESTS)
    add_subdirectory(test)
endif()

add_subdirectory(modules)
