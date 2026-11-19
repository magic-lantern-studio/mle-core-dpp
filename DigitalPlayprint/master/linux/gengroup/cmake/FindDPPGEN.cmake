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
  message(STATUS "  FindDPPGEN MLE_ROOT=" $ENV{MLE_ROOT})
else()
  message(STATUS "  FindDPPGEN MLE_ROOT NOT SET")
  set(ENV{MLE_ROOT} "/opt/MagicLantern")
  message(STATUS "  FindDPPGEN Using " $ENV{MLE_ROOT})
endif()

find_path(
  DPPGEN_INCLUDE_DIR
  NAMES DppGenCode.h
  PATHS $ENV{MLE_ROOT}/include/mle
  NO_CACHE)

find_library(
  DPPGEN_LIBRARY_DEBUG
  NAMES DPPGend
  PATHS $ENV{MLE_ROOT}/lib
  PATH_SUFFIXES mle)

find_library(
  DPPGEN_LIBRARY_RELEASE
  NAMES DPPGen
  PATHS $ENV{MLE_ROOT}/lib
  PATH_SUFFIXES mle)

select_library_configurations(DPPGEN)

set(DPPGEN_DEFINITIONS)
set(DPPGEN_INCLUDE_DIRS ${DPPGEN_INCLUDE_DIR})
set(DPPGEN_LIBRARIES ${DPPGEN_LIBRARY})

find_package_handle_standard_args(
  DPPGEN
  FOUND_VAR DPPGEN_FOUND
  REQUIRED_VARS DPPGEN_INCLUDE_DIR DPPGEN_LIBRARY)

if(DPPGEN_FOUND AND NOT TARGET DppGen::DppGen)
  add_library(DppGen::DppGen UNKNOWN IMPORTED)
  if(DPPGEN_LIBRARY_RELEASE)
    set_property(TARGET DppGen::DppGen APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
    set_target_properties(DppGen::DppGen PROPERTIES IMPORTED_LOCATION_RELEASE "${DPPGEN_LIBRARY_RELEASE}")
  endif()
  if(DPPGEN_LIBRARY_DEBUG)
    set_property(TARGET DppGen::DppGen APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
    set_target_properties(DppGen::DppGen PROPERTIES IMPORTED_LOCATION_DEBUG "${DPPGEN_LIBRARY_DEBUG}")
  endif()
  set_target_properties(DppGen::DppGen PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${DPPGEN_INCLUDE_DIRS}")
endif()

mark_as_advanced(DPPGEN_DEFINITIONS)
mark_as_advanced(DPPGEN_INCLUDE_DIRS)
mark_as_advanced(DPPGEN_LIBRARIES)
