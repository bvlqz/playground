# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/runner/work/playground/playground/build/_deps/imgui_external-src")
  file(MAKE_DIRECTORY "/home/runner/work/playground/playground/build/_deps/imgui_external-src")
endif()
file(MAKE_DIRECTORY
  "/home/runner/work/playground/playground/build/_deps/imgui_external-build"
  "/home/runner/work/playground/playground/build/_deps/imgui_external-subbuild/imgui_external-populate-prefix"
  "/home/runner/work/playground/playground/build/_deps/imgui_external-subbuild/imgui_external-populate-prefix/tmp"
  "/home/runner/work/playground/playground/build/_deps/imgui_external-subbuild/imgui_external-populate-prefix/src/imgui_external-populate-stamp"
  "/home/runner/work/playground/playground/build/_deps/imgui_external-subbuild/imgui_external-populate-prefix/src"
  "/home/runner/work/playground/playground/build/_deps/imgui_external-subbuild/imgui_external-populate-prefix/src/imgui_external-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/runner/work/playground/playground/build/_deps/imgui_external-subbuild/imgui_external-populate-prefix/src/imgui_external-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/runner/work/playground/playground/build/_deps/imgui_external-subbuild/imgui_external-populate-prefix/src/imgui_external-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
