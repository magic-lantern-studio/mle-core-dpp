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
project(libdpp VERSION 1.0.0 LANGUAGES C CXX)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify where to find CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Turn on verbose mode.
set(CMAKE_VERBOSE_MAKEFILE ON)

# Specify Debug postfix
set(CMAKE_DEBUG_POSTFIX "d")

message(STATUS "***** Processing libdpp *****")

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
  ../include
  ../../common/include
  $ENV{MLE_ROOT}/include)

# Specify the compiler macro definitions
add_compile_definitions(
  MLE_NOT_UTIL_DLL
  MLE_NOT_MATH_DLL
  MLE_NOT_DWP_DLL
  MLE_NOT_DPP_DLL)

# Specify the shared library
add_library(
  dwpShared SHARED
    ../../common/src/DPP/Dpp.cpp
    ../../common/src/DPPBase/DppItem.cpp
    ../../common/src/DPPBase/DppActorGroupOutput.cpp
    ../../common/src/DPPBase/DppSceneOutput.cpp
    ../../common/src/DPPBase/DppTblMgr.cpp
    ../../common/src/DPPType/DppArray.cpp
    ../../common/src/DPPType/DppEnum.cpp
    ../../common/src/DPPType/DppFloat.cpp
    ../../common/src/DPPType/DppFloat3.cpp
    ../../common/src/DPPType/DppFloat4.cpp
    ../../common/src/DPPType/DppFloatArray.cpp
    ../../common/src/DPPType/DppInt.cpp
    ../../common/src/DPPType/DppIntArray.cpp
    ../../common/src/DPPType/DppMatrix34.cpp
    ../../common/src/DPPType/DppRotation.cpp
    ../../common/src/DPPType/DppScalar.cpp
    ../../common/src/DPPType/DppScalarArray.cpp
    ../../common/src/DPPType/DppString.cpp
    ../../common/src/DPPType/DppTransform.cpp
    ../../common/src/DPPType/DppType.cpp
    ../../common/src/DPPType/DppVector2.cpp
    ../../common/src/DPPType/DppVector3.cpp
    ../../common/src/DPPType/DppVector3Array.cpp
    ../../common/src/DPPType/DppVector4.cpp
    ../../common/src/DPPType/DppVoidStarType.cpp
    ../../common/src/DPPModel/DppActor.cpp
    ../../common/src/DPPModel/DppActorSet.cpp
    ../../common/src/DPPModel/DppGroup.cpp
    ../../common/src/DPPModel/DppGroupRef.cpp
    ../../common/src/DPPModel/DppProperty.cpp
    ../../common/src/DPPModel/DppRoleAttachment.cpp
    ../../common/src/DPPModel/DppRoleBinding.cpp
    ../../common/src/DPPModel/DppScene.cpp
    ../../common/src/DPPModel/DppSet.cpp)

# Specify the static library
add_library(
  dwpStatic STATIC
    ../../common/src/DPP/Dpp.cpp
    ../../common/src/DPPBase/DppItem.cpp
    ../../common/src/DPPBase/DppActorGroupOutput.cpp
    ../../common/src/DPPBase/DppSceneOutput.cpp
    ../../common/src/DPPBase/DppTblMgr.cpp
    ../../common/src/DPPType/DppArray.cpp
    ../../common/src/DPPType/DppEnum.cpp
    ../../common/src/DPPType/DppFloat.cpp
    ../../common/src/DPPType/DppFloat3.cpp
    ../../common/src/DPPType/DppFloat4.cpp
    ../../common/src/DPPType/DppFloatArray.cpp
    ../../common/src/DPPType/DppInt.cpp
    ../../common/src/DPPType/DppIntArray.cpp
    ../../common/src/DPPType/DppMatrix34.cpp
    ../../common/src/DPPType/DppRotation.cpp
    ../../common/src/DPPType/DppScalar.cpp
    ../../common/src/DPPType/DppScalarArray.cpp
    ../../common/src/DPPType/DppString.cpp
    ../../common/src/DPPType/DppTransform.cpp
    ../../common/src/DPPType/DppType.cpp
    ../../common/src/DPPType/DppVector2.cpp
    ../../common/src/DPPType/DppVector3.cpp
    ../../common/src/DPPType/DppVector3Array.cpp
    ../../common/src/DPPType/DppVector4.cpp
    ../../common/src/DPPType/DppVoidStarType.cpp
    ../../common/src/DPPModel/DppActor.cpp
    ../../common/src/DPPModel/DppActorSet.cpp
    ../../common/src/DPPModel/DppGroup.cpp
    ../../common/src/DPPModel/DppGroupRef.cpp
    ../../common/src/DPPModel/DppProperty.cpp
    ../../common/src/DPPModel/DppRoleAttachment.cpp
    ../../common/src/DPPModel/DppRoleBinding.cpp
    ../../common/src/DPPModel/DppScene.cpp
    ../../common/src/DPPModel/DppSet.cpp)

  # Specify the shared library properties
  set_target_properties(dwpShared PROPERTIES
    OUTPUT_NAME dwp
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR})

  target_compile_options(dwpShared
    PRIVATE
      $<$<CONFIG:Debug>:-O0>
      $<$<CONFIG:Release>:>)

  target_compile_definitions(dwpShared
    PRIVATE
      $<$<CONFIG:Debug>: MLE_DEBUG DEBUG>
      $<$<CONFIG:Release>:>)

  # Specify the static library properties
  set_target_properties(dwpStatic PROPERTIES
    OUTPUT_NAME dwp
    VERSION ${PROJECT_VERSION})

  target_compile_options(dwpStatic
    PRIVATE
      $<$<CONFIG:Debug>:-O0>
      $<$<CONFIG:Release>:>)

  target_compile_definitions(dwpStatic
    PRIVATE
      $<$<CONFIG:Debug>: MLE_DEBUG>
      $<$<CONFIG:Release>:>)

  # Install the libraries
  install(
    TARGETS
      dwpShared
      dwpStatic
    DESTINATION
      lib)

  # Install the header files
  install(
    FILES
      ../../common/include/mle/agopcode.h
      ../../common/include/mle/Dpp.h
      ../../common/include/mle/DppActorGroupOutput.h
      ../../common/include/mle/DppActor.h
      ../../common/include/mle/DppActorSet.h
      ../../common/include/mle/DppArray.h
      ../../common/include/mle/DppDatatype.h
      ../../common/include/mle/DppEnum.h
      ../../common/include/mle/DppFloat3.h
      ../../common/include/mle/DppFloat4.h
      ../../common/include/mle/DppFloatArray.h
      ../../common/include/mle/DppFloat.h
      ../../common/include/mle/DppGroup.h
      ../../common/include/mle/DppGroupRef.h
      ../../common/include/mle/DppIntArray.h
      ../../common/include/mle/DppInt.h
      ../../common/include/mle/DppItem.h
      ../../common/include/mle/DppMatrix34.h
      ../../common/include/mle/DppProperty.h
      ../../common/include/mle/DppRoleAttachment.h
      ../../common/include/mle/DppRoleBinding.h
      ../../common/include/mle/DppRotation.h
      ../../common/include/mle/DppScalarArray.h
      ../../common/include/mle/DppScalar.h
      ../../common/include/mle/DppScene.h
      ../../common/include/mle/DppSceneOutput.h
      ../../common/include/mle/DppSet.h
      ../../common/include/mle/DppString.h
      ../../common/include/mle/DppTblMgr.h
      ../../common/include/mle/DppTransform.h
      ../../common/include/mle/DppType.h
      ../../common/include/mle/DppVector2.h
      ../../common/include/mle/DppVector3Array.h
      ../../common/include/mle/DppVector3.h
      ../../common/include/mle/DppVector4.h
      ../../common/include/mle/DppVoidStarType.h
    DESTINATION
      include/mle)
