include("CPM.cmake")
CPMAddPackage("GITHUB_REPOSITORY;juce-framework/JUCE;GIT_TAG;develop;EXCLUDE_FROM_ALL;YES")
set(JUCE_FOUND TRUE)