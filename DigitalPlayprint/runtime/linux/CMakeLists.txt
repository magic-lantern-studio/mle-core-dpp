# COPYRIGHT_BEGIN
#
# The MIT License (MIT)
#
# Copyright (c) 2025 Wizzer Works
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
#  For information concerning this header file, contact Mark S. Millard,
#  of Wizzer Works at msm@wizzerworks.com.
#
#  More information concerning Wizzer Works may be found at
#
#      http://www.wizzerworks.com
#
# COPYRIGHT_END

# Stop if cmake version is below 3.0
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# Project name and enable C++ support
project(mlert VERSION 1.0.0 LANGUAGES CXX)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify where to find CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Specify Debug postfix
set(CMAKE_DEBUG_POSTFIX "d")

# Fail early if one of the required packages cannot be found
include(FindMLUTIL)
find_package(MLUTIL REQUIRED)
include(FindMLMATH)
find_package(MLMATH REQUIRED)
include(FindDWP)
find_package(DWP REQUIRED)
find_package(PkgConfig REQUIRED)

# Check for Magic Lantern environment variables
if (DEFINED ENV{MLE_ROOT})
  message(STATUS "MLE_ROOT=" $ENV{MLE_ROOT})
else()
  message(STATUS "MLE_ROOT NOT SET")
  set(ENV{MLE_ROOT} "/opt/MagicLantern")
  message(STATUS "Using " $ENV{MLE_ROOT})
endif()

# Specify the library

# Specify the include directories for the header files
include_directories(
  include
  ../common/include
  ../../lib/common/include
  ../../lib/linux/include
  $ENV{MLE_ROOT}/include)

# Specify the compiler macro definitions
add_compile_definitions(
  MLE_NOT_UTIL_DLL
  MLE_NOT_MATH_DLL
  MLE_NOT_DWP_DLL)

# Specify the shared library
add_library(
  playprintShared SHARED
    ../common/src/agchk.cxx
    ../common/src/chkfile.cxx
    ../common/src/chktable.cxx
    ../common/src/mrefchk.cxx
    ../common/src/playprnt.c
    ../common/src/pp.cxx
    ../common/src/ppinput.cxx
    ../common/src/ppoutput.cxx
    ../common/src/scenechk.cxx)

# Specify the static library
add_library(
  playprintStatic STATIC
    ../common/src/agchk.cxx
    ../common/src/chkfile.cxx
    ../common/src/chktable.cxx
    ../common/src/mrefchk.cxx
    ../common/src/playprnt.c
    ../common/src/pp.cxx
    ../common/src/ppinput.cxx
    ../common/src/ppoutput.cxx
    ../common/src/scenechk.cxx)

  # Specify the shared library properties
  set_target_properties(playprintShared PROPERTIES
    OUTPUT_NAME playprint
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR})

  target_compile_options(playprintShared
    PRIVATE
      $<$<CONFIG:Debug>:-O0>
      $<$<CONFIG:Release>:>)

  target_compile_definitions(playprintShared
    PRIVATE
      $<$<CONFIG:Debug>: MLE_DEBUG>
      $<$<CONFIG:Release>:>)

  # Specify the static library properties
  set_target_properties(playprintStatic PROPERTIES
    OUTPUT_NAME playprint
    VERSION ${PROJECT_VERSION})

  target_compile_options(playprintStatic
    PRIVATE
      $<$<CONFIG:Debug>:-O0>
      $<$<CONFIG:Release>:>)

  target_compile_definitions(playprintStatic
    PRIVATE
      $<$<CONFIG:Debug>: MLE_DEBUG>
      $<$<CONFIG:Release>:>)

  # Install the libraries
  install(
    TARGETS
      playprintShared
      playprintStatic
    DESTINATION
      lib
   )

  # Install the header files
  install(
    FILES
      ../common/include/mle/playprnt.h
      ../common/include/mle/ppinput.h
      ../common/include/mle/pp.h
      ../common/include/mle/chunk.h
     DESTINATION
      include/mle
  )

  # Uninstall libraries and header files
  add_custom_target("uninstall" COMMENT "Uninstall installed files")
  add_custom_command(
    TARGET "uninstall"
    POST_BUILD
    COMMENT "Uninstall files with install_manifest.txt"
    COMMAND xargs rm -vf < install_manifest.txt || echo Nothing in
            install_manifest.txt to be uninstalled!
  )
