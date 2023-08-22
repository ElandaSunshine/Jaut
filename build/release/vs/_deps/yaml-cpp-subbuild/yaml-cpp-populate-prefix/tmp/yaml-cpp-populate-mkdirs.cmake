# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/yaml-cpp-src"
  "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/yaml-cpp-build"
  "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/yaml-cpp-subbuild/yaml-cpp-populate-prefix"
  "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/yaml-cpp-subbuild/yaml-cpp-populate-prefix/tmp"
  "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/yaml-cpp-subbuild/yaml-cpp-populate-prefix/src/yaml-cpp-populate-stamp"
  "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/yaml-cpp-subbuild/yaml-cpp-populate-prefix/src"
  "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/yaml-cpp-subbuild/yaml-cpp-populate-prefix/src/yaml-cpp-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Development/Coding/cpp/Jaut/build/release/vs/_deps/yaml-cpp-subbuild/yaml-cpp-populate-prefix/src/yaml-cpp-populate-stamp/${subDir}")
endforeach()
