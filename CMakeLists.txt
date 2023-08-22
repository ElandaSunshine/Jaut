########################################################################################################################
cmake_minimum_required(VERSION 3.22 FATAL_ERROR)



########################################################################################################################
project(JAUT
    VERSION   0.1.0
    LANGUAGES C CXX)



########################################################################################################################
include(${JAUT_SOURCE_DIR}/cmake/CPM.cmake)



########################################################################################################################
set(CMAKE_CXX_STANDARD            17)
set(CMAKE_CXX_STANDARD_REQUIRED   ON)



########################################################################################################################
option(JAUT_BUILD_TESTS    "Build unit tests for the JAUT bundle" OFF)
option(JAUT_BUILD_EXAMPLES "Build examples for the JAUT bundle" OFF)
option(JAUT_CLONE_JUCE     "Whether JUCE should be cloned for JAUT specifically, this will majorly be used for standalone development of the module bundle" OFF)
mark_as_advanced(JAUT_CLONE_JUCE)



########################################################################################################################
if (JAUT_CLONE_JUCE)
    message(VERBOSE "[JAUT] Fetching JUCE for JAUT development")
    CPMAddPackage("gh:juce-framework/JUCE#develop")
endif()

if (JAUT_BUILD_TESTS)
    include(CTest)
    add_subdirectory(test)
endif()
    
if (JAUT_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
    
add_subdirectory(modules)
