# Install script for directory: D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-src

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
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
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

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/Users/elend/AppData/Local/JetBrains/Toolbox/apps/CLion/ch-0/222.4345.21/bin/mingw/bin/objdump.exe")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-build/tools/modules/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-build/tools/extras/Build/cmake_install.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/JUCE-7.0.5" TYPE FILE FILES
    "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-build/tools/JUCEConfigVersion.cmake"
    "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-build/tools/JUCEConfig.cmake"
    "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-src/extras/Build/CMake/JUCECheckAtomic.cmake"
    "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-src/extras/Build/CMake/JUCEHelperTargets.cmake"
    "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-src/extras/Build/CMake/JUCEModuleSupport.cmake"
    "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-src/extras/Build/CMake/JUCEUtils.cmake"
    "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-src/extras/Build/CMake/JuceLV2Defines.h.in"
    "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-src/extras/Build/CMake/LaunchScreen.storyboard"
    "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-src/extras/Build/CMake/PIPAudioProcessor.cpp.in"
    "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-src/extras/Build/CMake/PIPAudioProcessorWithARA.cpp.in"
    "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-src/extras/Build/CMake/PIPComponent.cpp.in"
    "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-src/extras/Build/CMake/PIPConsole.cpp.in"
    "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-src/extras/Build/CMake/RecentFilesMenuTemplate.nib"
    "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-src/extras/Build/CMake/UnityPluginGUIScript.cs.in"
    "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-src/extras/Build/CMake/checkBundleSigning.cmake"
    "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-src/extras/Build/CMake/copyDir.cmake"
    "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-src/extras/Build/CMake/juce_runtime_arch_detection.cpp"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/JUCE-7.0.5" TYPE EXECUTABLE FILES "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-build/tools/juce_lv2_helper.exe")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/JUCE-7.0.5/juce_lv2_helper.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/JUCE-7.0.5/juce_lv2_helper.exe")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/Users/elend/AppData/Local/JetBrains/Toolbox/apps/CLion/ch-0/222.4345.21/bin/mingw/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/JUCE-7.0.5/juce_lv2_helper.exe")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/JUCE-7.0.5/LV2_HELPER.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/JUCE-7.0.5/LV2_HELPER.cmake"
         "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-build/tools/CMakeFiles/Export/lib/cmake/JUCE-7.0.5/LV2_HELPER.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/JUCE-7.0.5/LV2_HELPER-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/JUCE-7.0.5/LV2_HELPER.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/JUCE-7.0.5" TYPE FILE FILES "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-build/tools/CMakeFiles/Export/lib/cmake/JUCE-7.0.5/LV2_HELPER.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/JUCE-7.0.5" TYPE FILE FILES "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-build/tools/CMakeFiles/Export/lib/cmake/JUCE-7.0.5/LV2_HELPER-debug.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-build/tools/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
