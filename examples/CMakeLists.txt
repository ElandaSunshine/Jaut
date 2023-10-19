########################################################################################################################
# Example setup
# The example list
set(JAUT_EXAMPLE_LIST "" CACHE STRING "The list of examples to build")
set(JAUT_BUILD_ALL_EXAMPLES TRUE)

if ("${JAUT_EXAMPLE_LIST}" STREQUAL "[^ ]+")
    set(JAUT_BUILD_ALL_EXAMPLES FALSE)
endif()

if (WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/bigobj)
endif()



########################################################################################################################
function(jaut_add_example target)
    if (NOT JAUT_BUILD_ALL_EXAMPLES AND NOT "${target}" IN_LIST JAUT_EXAMPLE_LIST)
        return()
    endif()
    
    cmake_parse_arguments(PARG "GUI_APP" "" "DEPENDENCIES;ASSETS;RESOURCES;DEFINES" ${ARGN})
    
    string(TOUPPER ${target} EXAMPLE_NAME)
    set(EXAMPLE_TARGET Example${EXAMPLE_NAME})
    
    if (NOT PARG_GUI_APP)
        add_executable(${EXAMPLE_TARGET} ${Jaut_SOURCE_DIR}/examples/${target}.cpp)
        target_compile_definitions(${EXAMPLE_TARGET}
            PRIVATE
                JUCE_STANDALONE_APPLICATION=1
                JUCE_USE_CURL=0
                JUCE_USE_WEB_BROWSER=0
                APP_NAME="${target}"
                APP_VERSION="1.0")
        
        if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
            target_compile_definitions(${EXAMPLE_TARGET}
                PRIVATE
                    _CONSOLE=1)
        endif()
        
        _juce_initialise_target(${EXAMPLE_TARGET}
            NEEDS_BROWSER FALSE
            NEEDS_CURL    FALSE)
    else()
        set(SOURCE_LIST
            ${Jaut_SOURCE_DIR}/examples/${target}.cpp
            ${Jaut_SOURCE_DIR}/examples/detail/GuiBase.cpp)
        
        if(CMAKE_SYSTEM_NAME STREQUAL "Android")
            add_library(${EXAMPLE_TARGET} SHARED ${SOURCE_LIST})
        else()
            add_executable(${EXAMPLE_TARGET} ${SOURCE_LIST})
        endif()
        
        target_compile_definitions(${EXAMPLE_TARGET}
            PRIVATE
                JUCE_STANDALONE_APPLICATION=1
                APP_NAME="${target}"
                APP_VERSION="1.0")
        
        _juce_initialise_target(${EXAMPLE_TARGET}
            NEEDS_BROWSER FALSE
            NEEDS_CURL    FALSE)
        
        _juce_set_output_name(${EXAMPLE_TARGET} $<TARGET_PROPERTY:${EXAMPLE_TARGET},JUCE_PRODUCT_NAME>)
        
        set_target_properties(${EXAMPLE_TARGET}
            PROPERTIES
                JUCE_TARGET_KIND_STRING "App")
        
        _juce_configure_bundle(${EXAMPLE_TARGET} ${EXAMPLE_TARGET})
        _juce_configure_app_bundle(${EXAMPLE_TARGET} ${EXAMPLE_TARGET})
    endif()
    
    if (DEFINED PARG_ASSETS AND NOT "ASSETS" IN_LIST PARG_KEYWORDS_MISSING_VALUES)
        set(BINARY_DATA_TARGET ${EXAMPLE_TARGET}_Assets)
        juce_add_binary_data(${BINARY_DATA_TARGET}
            SOURCES
                ${PARG_ASSETS})
    endif()
    
    if (DEFINED PARG_RESOURCES AND NOT "RESOURCES" IN_LIST PARG_KEYWORDS_MISSING_VALUES)
        foreach (RESOURCE IN LISTS PARG_RESOURCES)
            get_filename_component(FILE_OUT_NAME ${RESOURCE} NAME)
            
            list(APPEND RESOURCE_NAMES "${FILE_OUT_NAME}")
            list(APPEND RESOURCE_PATHS "${Jaut_SOURCE_DIR}/example/assets/${RESOURCE}")
        endforeach()
                
        add_custom_command(
            OUTPUT 
                ${RESOURCE_NAMES}
                
            COMMAND
                ${CMAKE_COMMAND} -E copy_if_different
                    ${RESOURCE_PATHS}
                    $<TARGET_FILE_DIR:${EXAMPLE_TARGET}>
                    
            DEPENDS
                ${RESOURCE_PATHS}
                
            WORKING_DIRECTORY
                ${CMAKE_CURRENT_LIST_DIR}
                
            VERBATIM)
        
        add_custom_target(${EXAMPLE_TARGET}_Resources
            DEPENDS ${RESOURCE_NAMES})
        add_dependencies(${EXAMPLE_TARGET} ${EXAMPLE_TARGET}_Resources)
    endif()
    
    if (DEFINED PARG_DEFINES AND NOT "DEFINES" IN_LIST PARG_KEYWORDS_MISSING_VALUES)
        target_compile_definitions(${EXAMPLE_TARGET}
            PRIVATE
                ${PARG_DEFINES})
    endif()
    
    target_link_libraries(${EXAMPLE_TARGET}
        PRIVATE                  
            # JUCE recommended targets
            juce::juce_recommended_config_flags
            juce::juce_recommended_lto_flags
            juce::juce_recommended_warning_flags
            
            ${BINARY_DATA_TARGET}
            ${PARG_DEPENDENCIES})
endfunction()



########################################################################################################################
##jaut_add_example(GuiExamples GUI_APP
##    DEPENDENCIES
##        jaut::jaut_gui)

jaut_add_example(Config
    DEPENDENCIES
        jaut::jaut_provider)
