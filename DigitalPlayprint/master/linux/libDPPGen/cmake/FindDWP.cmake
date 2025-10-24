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

include(FindPackageHandleStandardArgs)
include(SelectLibraryConfigurations)

if (DEFINED ENV{MLE_ROOT})
  message(STATUS "  FindDWP MLE_ROOT=" $ENV{MLE_ROOT})
else()
  message(STATUS "  FindDWP MLE_ROOT NOT SET")
  set(ENV{MLE_ROOT} "/opt/MagicLantern")
  message(STATUS "  FindDWP Using " $ENV{MLE_ROOT})
endif()

find_path(
  DWP_INCLUDE_DIR
  NAMES Dwp.h
  PATHS $ENV{MLE_ROOT}/include/mle
  NO_CACHE)

find_library(
  DWP_LIBRARY_DEBUG
  NAMES dwpd
  PATHS $ENV{MLE_ROOT}/lib
  PATH_SUFFIXES mle)

find_library(
  DWP_LIBRARY_RELEASE
  NAMES dwp
  PATHS $ENV{MLE_ROOT}/lib
  PATH_SUFFIXES mle)

select_library_configurations(DWP)

#message(STATUS "  FindDWP DWP_INCLUDE_DIR=" ${DWP_INCLUDE_DIR})
#message(STATUS "  FindDWP DWP_LIBRARY_DEBUG=" ${DWP_LIBRARY_DEBUG})
#message(STATUS "  FindDWP DWP_LIBRARY_RELEASE=" ${DWP_LIBRARY_RELEASE})

if(MLMath_INCLUDE_DIR AND EXISTS "${DWP_INCLUDE_DIR}/Dwp.h")
  file(STRINGS "${DWP_INCLUDE_DIR}/Dwp.h" _DWP_VERSION_DEFINE REGEX "[\t ]*#define[\t ]+DWP_VERSION[\t ]+\"[^\"]*\".*")
  string(REGEX REPLACE "[\t ]*#define[\t ]+DWP_VERSION[\t ]+\"([^\"]*)\".*" "\\1" DWP_VERSION "${_DWP_VERSION_DEFINE}")
  unset(_DWP_VERSION_DEFINE)
endif()

set(DWP_DEFINITIONS)
set(DWP_INCLUDE_DIRS ${DWP_INCLUDE_DIR})
set(DWP_LIBRARIES ${DWP_LIBRARY})

find_package_handle_standard_args(
  DWP
  FOUND_VAR DWP_FOUND
  REQUIRED_VARS DWP_INCLUDE_DIR DWP_LIBRARY
  VERSION_VAR DWP_VERSION)

if(DWP_FOUND AND NOT TARGET Dwp::Dwp)
  add_library(Dwp::Dwp UNKNOWN IMPORTED)
  if(DWP_LIBRARY_RELEASE)
    set_property(TARGET Dwp::Dwp APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
    set_target_properties(Dwp::Dwp PROPERTIES IMPORTED_LOCATION_RELEASE "${DWP_LIBRARY_RELEASE}")
  endif()
  if(DWP_LIBRARY_DEBUG)
    set_property(TARGET Dwp::Dwp APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
    set_target_properties(Dwp::Dwp PROPERTIES IMPORTED_LOCATION_DEBUG "${DWP_LIBRARY_DEBUG}")
  endif()
  set_target_properties(Dwp::Dwp PROPERTIES INTERFACE_COMPILE_DEFINITIONS "COIN_DLL")
  set_target_properties(Dwp::Dwp PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${DWP_INCLUDE_DIRS}")
endif()

mark_as_advanced(DWP_DEFINITIONS)
mark_as_advanced(DWP_INCLUDE_DIRS)
mark_as_advanced(DWP_LIBRARIES)
