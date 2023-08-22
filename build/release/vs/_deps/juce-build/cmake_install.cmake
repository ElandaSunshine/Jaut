# Install script for directory: D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/juce-src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/JAUT")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/juce-build/modules/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/juce-build/extras/Build/cmake_install.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/JUCE-7.0.5" TYPE FILE FILES
    "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/juce-build/JUCEConfigVersion.cmake"
    "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/juce-build/JUCEConfig.cmake"
    "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/juce-src/extras/Build/CMake/JUCECheckAtomic.cmake"
    "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/juce-src/extras/Build/CMake/JUCEHelperTargets.cmake"
    "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/juce-src/extras/Build/CMake/JUCEModuleSupport.cmake"
    "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/juce-src/extras/Build/CMake/JUCEUtils.cmake"
    "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/juce-src/extras/Build/CMake/JuceLV2Defines.h.in"
    "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/juce-src/extras/Build/CMake/LaunchScreen.storyboard"
    "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/juce-src/extras/Build/CMake/PIPAudioProcessor.cpp.in"
    "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/juce-src/extras/Build/CMake/PIPAudioProcessorWithARA.cpp.in"
    "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/juce-src/extras/Build/CMake/PIPComponent.cpp.in"
    "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/juce-src/extras/Build/CMake/PIPConsole.cpp.in"
    "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/juce-src/extras/Build/CMake/RecentFilesMenuTemplate.nib"
    "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/juce-src/extras/Build/CMake/UnityPluginGUIScript.cs.in"
    "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/juce-src/extras/Build/CMake/checkBundleSigning.cmake"
    "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/juce-src/extras/Build/CMake/copyDir.cmake"
    "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/juce-src/extras/Build/CMake/juce_runtime_arch_detection.cpp"
    )
endif()

