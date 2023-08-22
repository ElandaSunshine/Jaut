########################################################################################################################
# Modules
include(CTest)



########################################################################################################################
# Test dependencies
CPMAddPackage("gh:google/googletest#main")
CPMAddPackage("gh:jbeder/yaml-cpp#master")



########################################################################################################################
# Test setup
# The test list
set(JAUT_TEST_LIST "" CACHE STRING "The list of tests to run")
set(JAUT_TEST_ALL TRUE)

if ("${JAUT_TEST_LIST}" STREQUAL "[^ ]+")
    set(JAUT_TEST_ALL FALSE)
endif()

if (WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/bigobj)
endif()



########################################################################################################################
function(jaut_add_test target group)
    if (NOT JAUT_TEST_ALL AND NOT "${target}" IN_LIST JAUT_TEST_LIST)
        return()
    endif()
    
    cmake_parse_arguments(PARG "" "" "DEPENDENCIES;ASSETS;RESOURCES;DEFINES" ${ARGN})
    
    string(TOUPPER ${target} TEST_NAME)
    set(TEST_TARGET Test${TEST_NAME})
    
    add_executable(${TEST_TARGET} ${JAUT_SOURCE_DIR}/test/${group}/${target}.cpp)
    target_compile_definitions(${TEST_TARGET}
        PRIVATE
            JUCE_STANDALONE_APPLICATION=1
            JUCE_USE_CURL=0
            JUCE_USE_WEB_BROWSER=0)
    
    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        target_compile_definitions(${TEST_TARGET}
            PRIVATE
                _CONSOLE=1)
    endif()
    
    _juce_initialise_target(${TEST_TARGET}
        NEEDS_BROWSER FALSE
        NEEDS_CURL    FALSE)
    
    if (DEFINED PARG_ASSETS AND NOT "ASSETS" IN_LIST PARG_KEYWORDS_MISSING_VALUES)
        set(BINARY_DATA_TARGET ${TEST_TARGET}_Assets)
        juce_add_binary_data(${BINARY_DATA_TARGET}
            SOURCES
                ${PARG_ASSETS})
    endif()
    
    if (DEFINED PARG_RESOURCES AND NOT "RESOURCES" IN_LIST PARG_KEYWORDS_MISSING_VALUES)
        foreach (RESOURCE IN LISTS PARG_RESOURCES)
            get_filename_component(FILE_OUT_NAME ${RESOURCE} NAME)
            
            list(APPEND RESOURCE_NAMES "${FILE_OUT_NAME}")
            list(APPEND RESOURCE_PATHS "${JAUT_SOURCE_DIR}/test/assets/${RESOURCE}")
        endforeach()
                
        add_custom_command(
            OUTPUT 
                ${RESOURCE_NAMES}
                
            COMMAND
                ${CMAKE_COMMAND} -E copy_if_different
                    ${RESOURCE_PATHS}
                    $<TARGET_FILE_DIR:${TEST_TARGET}>
                    
            DEPENDS
                ${RESOURCE_PATHS}
                
            WORKING_DIRECTORY
                ${CMAKE_CURRENT_LIST_DIR}
                
            VERBATIM)
        
        add_custom_target(${TEST_TARGET}_Resources
            DEPENDS ${RESOURCE_NAMES})
        add_dependencies(${TEST_TARGET} ${TEST_TARGET}_Resources)
    endif()
    
    if (DEFINED PARG_DEFINES AND NOT "DEFINES" IN_LIST PARG_KEYWORDS_MISSING_VALUES)
        target_compile_definitions(${TEST_TARGET}
            PRIVATE
                ${PARG_DEFINES})
    endif()
    
    target_link_libraries(${TEST_TARGET}
        PRIVATE
            # GTEST
            gtest
            gtest_main
            
            # Dummy module, to include all module headers
            jaut::jaut_dummy
            
            # JUCE recommended targets
            juce::juce_recommended_config_flags
            juce::juce_recommended_lto_flags
            juce::juce_recommended_warning_flags
            
            ${BINARY_DATA_TARGET}
            ${PARG_DEPENDENCIES})

    add_test(NAME    ${TEST_TARGET}
             COMMAND ${TEST_TARGET})
endfunction()



########################################################################################################################
# Core test
jaut_add_test(Event core
    DEPENDENCIES
        juce::juce_core)

jaut_add_test(Numeric core
    DEPENDENCIES
        juce::juce_core)

jaut_add_test(Preprocessor core
    DEPENDENCIES
        juce::juce_core)

jaut_add_test(Stringable core
    DEPENDENCIES
        juce::juce_core)

jaut_add_test(TypeTraits core
    DEPENDENCIES
        juce::juce_core)

jaut_add_test(Version core
    DEPENDENCIES
        juce::juce_core)

# Provider test
jaut_add_test(Config provider
    DEPENDENCIES
        yaml-cpp
        jaut::jaut_core)

# Message test
jaut_add_test(MessageHandler message
    DEPENDENCIES
        jaut::jaut_core
        juce::juce_events)

# Logger test
jaut_add_test(Logger logger
    DEPENDENCIES
        jaut::jaut_message
        juce::juce_graphics)

jaut_add_test(LoggerSink logger
    DEPENDENCIES
        jaut::jaut_message
        juce::juce_graphics)

jaut_add_test(LoggerFormat logger
    DEPENDENCIES
        jaut::jaut_message
        juce::juce_graphics)


