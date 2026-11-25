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

# Stop if cmake version is below 3.10
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# Project name and enable C++ support
project(libDPP-test VERSION 1.0.0 LANGUAGES CXX)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check for Magic Lantern environment variables
if (DEFINED ENV{MLE_ROOT})
  message(STATUS "MLE_ROOT=" $ENV{MLE_ROOT})
else()
  message(STATUS "MLE_ROOT NOT SET")
  set(ENV{MLE_ROOT} "/opt/MagicLantern")
  message(STATUS "Using " $ENV{MLE_ROOT})
endif()

# Fail early if one of the required packages cannot be found. The tests are
# run against libdpp, libplayprint, libDPPGen, libdwp, libmlmath and libmlutil,
# which must already be installed in $MLE_ROOT. A Debug build uses the Debug
# libraries (the name ends in "d"), the other builds the Release libraries.
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
foreach(lib dpp playprint DPPGen dwp mlmath mlutil)
  string(TOUPPER ${lib} upper)
  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(names ${lib}d ${lib})
  else()
    set(names ${lib} ${lib}d)
  endif()
  find_library(${upper}_LIBRARY
    NAMES ${names}
    HINTS $ENV{MLE_ROOT}/lib
    NO_DEFAULT_PATH)
  if (NOT ${upper}_LIBRARY)
    message(FATAL_ERROR
      "Could not find lib${lib} in $ENV{MLE_ROOT}/lib. "
      "Install mle-core-util, mle-core-math, mle-core-dwp and mle-core-dpp into MLE_ROOT first.")
  endif()
  message(STATUS "Using ${${upper}_LIBRARY}")
endforeach()

# Specify the compiler macro definitions. MLE_DEBUG must match the library
# build because it changes the layout of some of its classes.
add_compile_definitions(
  MLE_NOT_UTIL_DLL
  MLE_NOT_MATH_DLL
  MLE_NOT_DWP_DLL
  MLE_NOT_DPP_DLL
  $<$<CONFIG:Debug>:MLE_DEBUG>
  $<$<CONFIG:Debug>:DEBUG>)

# Specify the include directories for the header files. The headers of
# libplayprint and libDPPGen are found in the source tree, as they are not
# installed.
include_directories(
  include
  ../../../master/common/include
  ../../../runtime/common/include
  $ENV{MLE_ROOT}/include)

# The unit test library, libdpptest. It is not linked against Google Test or
# the Magic Lantern libraries; the symbols are resolved by exampleProgram,
# which loads it.
add_library(dpptest SHARED
  libdpptest/libdpptest.cxx
  libdpptest/fixtures.cxx
  libdpptest/testInfrastructure.cxx
  libdpptest/testSwap.cxx
  libdpptest/testChunk.cxx
  libdpptest/testPlayprint.cxx
  libdpptest/testChunkFiles.cxx
  libdpptest/testCheckers.cxx
  libdpptest/testBadInput.cxx
  libdpptest/testChunkMarker.cxx
  libdpptest/testGenMakefile.cxx
  libdpptest/testGenPpscript.cxx
  libdpptest/testTitleAnt.cxx)
target_include_directories(dpptest PRIVATE
  $<TARGET_PROPERTY:GTest::gtest,INTERFACE_INCLUDE_DIRECTORIES>)

# The tests find the test files by absolute path.
target_compile_definitions(dpptest PRIVATE
  DPPTEST_WORKPRINT_DIR="${CMAKE_CURRENT_SOURCE_DIR}/workprints"
  DPPTEST_GOLDEN_DIR="${CMAKE_CURRENT_SOURCE_DIR}/golden")

# The unit test runner.
add_executable(exampleProgram exampleProgram/exampleProgram.cxx)
target_link_libraries(exampleProgram
  dpptest
  ${DPP_LIBRARY}
  ${PLAYPRINT_LIBRARY}
  ${DPPGEN_LIBRARY}
  ${DWP_LIBRARY}
  ${MLMATH_LIBRARY}
  ${MLUTIL_LIBRARY}
  GTest::gtest
  ${CMAKE_DL_LIBS}
  Threads::Threads)

# Register each test with ctest, so a failure is reported for the test that
# failed and tests can be selected with "ctest -R".
include(GoogleTest)
enable_testing()
gtest_discover_tests(exampleProgram
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DISCOVERY_TIMEOUT 30)
