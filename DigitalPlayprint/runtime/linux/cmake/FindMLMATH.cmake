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
  message(STATUS "  FindMLMATH MLE_ROOT=" $ENV{MLE_ROOT})
else()
  message(STATUS "  FindMLMATH MLE_ROOT NOT SET")
  set(ENV{MLE_ROOT} "/opt/MagicLantern")
  message(STATUS "  FindMLMATH Using " $ENV{MLE_ROOT})
endif()

find_path(
  MLMATH_INCLUDE_DIR
  NAMES mlmath.h
  PATHS $ENV{MLE_ROOT}/include/math
  NO_CACHE)

find_library(
  MLMATH_LIBRARY_DEBUG
  NAMES mlmathd
  PATHS $ENV{MLE_ROOT}/lib
  PATH_SUFFIXES mle)

find_library(
  MLMATH_LIBRARY_RELEASE
  NAMES mlmath
  PATHS $ENV{MLE_ROOT}/lib
  PATH_SUFFIXES mle)

#message(STATUS "  FindMLMATH MLMATH_INCLUDE_DIR=" ${MLMATH_INCLUDE_DIR})
#message(STATUS "  FindMLMATH MLMATH_LIBRARY_DEBUG=" ${MLMATH_LIBRARY_DEBUG})
#message(STATUS "  FindMLMATH MLMATH_LIBRARY_RELEASE=" ${MLMATH_LIBRARY_RELEASE})

select_library_configurations(MLMATH)

if(MLMATH_INCLUDE_DIR AND EXISTS "${MLMATH_INCLUDE_DIR}/mlmath.h")
  file(STRINGS "${MLMATH_INCLUDE_DIR}/mlmath.h" _MLMATH_VERSION_DEFINE REGEX "[\t ]*#define[\t ]+MLMATH_VERSION[\t ]+\"[^\"]*\".*")
  string(REGEX REPLACE "[\t ]*#define[\t ]+MLMATH_VERSION[\t ]+\"([^\"]*)\".*" "\\1" MLMATH_VERSION "${_MLMATH_VERSION_DEFINE}")
  unset(_MLMATH_VERSION_DEFINE)
endif()

set(MLMATH_DEFINITIONS)
set(MLMATH_INCLUDE_DIRS ${MLMATH_INCLUDE_DIR})
set(MLMATH_LIBRARIES ${MLMATH_LIBRARY})

find_package_handle_standard_args(
  MLMATH
  FOUND_VAR MLMATH_FOUND
  REQUIRED_VARS MLMATH_INCLUDE_DIR MLMATH_LIBRARY
  VERSION_VAR MLMATH_VERSION)

#if(MLMATH_FOUND AND NOT TARGET MLMath::MLMath)
#  add_library(MlMath::MlMath UNKNOWN IMPORTED)
#  if(MLMATH_LIBRARY_RELEASE)
#    set_property(TARGET MlMath::MlMath APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
#    set_target_properties(MlMath::MlMath PROPERTIES IMPORTED_LOCATION_RELEASE "${MLMATH_LIBRARY_RELEASE}")
#  endif()
#  if(MLMATH_LIBRARY_DEBUG)
#    set_property(TARGET MlMath::MlMath APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
#    set_target_properties(MlMath::MlMath PROPERTIES IMPORTED_LOCATION_DEBUG "${MLMATH_LIBRARY_DEBUG}")
#  endif()
#  set_target_properties(MlMath::MlMath PROPERTIES INTERFACE_COMPILE_DEFINITIONS "")
#  set_target_properties(MlMath::MlMath PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${MLMATH_INCLUDE_DIRS}")
#endif()

mark_as_advanced(MLMATH_DEFINITIONS)
mark_as_advanced(MLMATH_INCLUDE_DIRS)
mark_as_advanced(MLMATH_LIBRARIES)
