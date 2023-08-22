########################################################################################################################
# This is heavily inspired and copied by JUCE
# https://github.com/juce-framework/JUCE/blob/master/extras/Build/juceaide/CMakeLists.txt
# Look there if you don't know what this does (I do neither, really ¯\_(ツ)_/¯)
if (JAUT_BUILD_JAUTGEN)
    juce_add_console_app(jautgen)
    
    target_sources(jautgen
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/main.cpp)
    
    target_compile_definitions(jautgen
        PRIVATE
            JUCE_DISABLE_JUCE_VERSION_PRINTING=1
            JUCE_USE_CURL=0)
    
    target_link_libraries(jautgen
        PRIVATE
            juce::juce_core
            
            juce::juce_recommended_config_flags
            juce::juce_recommended_lto_flags
            juce::juce_recommended_warning_flags)
    
    set_target_properties(jautgen PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    
    export(TARGETS jautgen
       NAMESPACE jautextra::
       FILE "${JAUT_BINARY_DIR}/tools/ExportJautgen.cmake")
else()
    message(STATUS "[JAUT] Configuring jautgen...")
    
    if (CMAKE_CROSSCOMPILING)
        unset(ENV{ASM})
        unset(ENV{CC})
        unset(ENV{CXX})
    else()
        set(extra_configure_flags
            "-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}"
            "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}"
            "-DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}"
            "-DCMAKE_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}")
    endif()
    
    execute_process(
        COMMAND
            "${CMAKE_COMMAND}"
                "."
                "-B${JAUT_BINARY_DIR}"
                "-G${CMAKE_GENERATOR}"
                "-DCMAKE_BUILD_TYPE=Debug"
                "-DJAUT_BUILD_JAUTGEN=ON"
                "-DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}"
                "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
                "-DJAUT_CLONE_JUCE=${JAUT_CLONE_JUCE}"
                ${extra_configure_flags}
        WORKING_DIRECTORY "${JAUT_SOURCE_DIR}"
        OUTPUT_VARIABLE   JAUT_TOOL_OUT
        ERROR_VARIABLE    JAUT_TOOL_OUT
        RESULT_VARIABLE   JAUT_RES_OUT)
    
    if (JAUT_RES_OUT)
        message(FATAL_ERROR "[JAUT] Couldn't configure jautgen: ${JAUT_TOOL_OUT}")
    endif()
    
    message(STATUS "[JAUT] Building jautgen...")
    
    execute_process(
        COMMAND
            "${CMAKE_COMMAND}"
                --build "${JAUT_BINARY_DIR}"
                --config Debug
                --target jautgen
        OUTPUT_VARIABLE JAUT_TOOL_OUT
        ERROR_VARIABLE  JAUT_TOOL_OUT
        RESULT_VARIABLE JAUT_RES_OUT)
    
    if (JAUT_RES_OUT)
        message(FATAL_ERROR "[JAUT] Couldn't build jautgen: ${JAUT_TOOL_OUT}")
    endif()
    
    message(STATUS "[JAUT] Exporting jautgen...")
    
    include("${JAUT_BINARY_DIR}/tools/ExportJautgen.cmake")
    
    add_executable(jautgen IMPORTED GLOBAL)
    
    get_target_property(JAUT_IMPORTED_LOCATION jautextra::jautgen IMPORTED_LOCATION_DEBUG)
    set_target_properties(jautgen PROPERTIES IMPORTED_LOCATION "${JAUT_IMPORTED_LOCATION}")
    
    add_executable(jaut::jautgen ALIAS jautgen)
    
    set(JAUT_TOOL_INSTALL_DIR "bin/jaut/${JAUT_VERSION}" CACHE STRING "The installation dir for jautgen")
    
    install(PROGRAMS "${JAUT_IMPORTED_LOCATION}" DESTINATION "${JAUT_TOOL_INSTALL_DIR}")
    
    get_filename_component(JAUT_BINARY_NAME "${JAUT_IMPORTED_LOCATION}" NAME)
    set(JAUT_JAUTGEN_NAME "${JAUT_BINARY_NAME}" CACHE INTERNAL "")
endif()
