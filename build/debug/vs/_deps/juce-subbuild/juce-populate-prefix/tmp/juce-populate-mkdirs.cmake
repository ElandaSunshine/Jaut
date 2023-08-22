# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-src"
  "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-build"
  "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-subbuild/juce-populate-prefix"
  "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-subbuild/juce-populate-prefix/tmp"
  "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-subbuild/juce-populate-prefix/src/juce-populate-stamp"
  "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-subbuild/juce-populate-prefix/src"
  "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-subbuild/juce-populate-prefix/src/juce-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Development/Coding/cpp/Jaut/build/debug/vs/_deps/juce-subbuild/juce-populate-prefix/src/juce-populate-stamp/${subDir}")
endforeach()
