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
  message(STATUS "  FindMLUTIL MLE_ROOT=" $ENV{MLE_ROOT})
else()
  message(STATUS "  FindMLUTIL MLE_ROOT NOT SET")
  set(ENV{MLE_ROOT} "/opt/MagicLantern")
  message(STATUS "  FindMLUTIL Using " $ENV{MLE_ROOT})
endif()

find_path(
  MLUTIL_INCLUDE_DIR
  NAMES mle.h
  PATHS $ENV{MLE_ROOT}/include/mle
  NO_CACHE)

find_library(
  MLUTIL_LIBRARY_DEBUG
  NAMES mlutild
  PATHS $ENV{MLE_ROOT}/lib
  PATH_SUFFIXES mle)

find_library(
  MLUTIL_LIBRARY_RELEASE
  NAMES mlutil
  PATHS $ENV{MLE_ROOT}/lib
  PATH_SUFFIXES mle)

select_library_configurations(MLUTIL)

#message(STATUS "  FindMLUTIL MLUTIL_INCLUDE_DIR=" ${MLUTIL_INCLUDE_DIR})
#message(STATUS "  FindMLUTIL MLUTIL_LIBRARY_DEBUG=" ${MLUTIL_LIBRARY_DEBUG})
#message(STATUS "  FindMLUTIL MLUTIL_LIBRARY_RELEASE=" ${MLUTIL_LIBRARY_RELEASE})

if(MLMath_INCLUDE_DIR AND EXISTS "${MLUTIL_INCLUDE_DIR}/mlutil.h")
  file(STRINGS "${MLUTIL_INCLUDE_DIR}/mlutil.h" _MLUTIL_VERSION_DEFINE REGEX "[\t ]*#define[\t ]+MLUTIL_VERSION[\t ]+\"[^\"]*\".*")
  string(REGEX REPLACE "[\t ]*#define[\t ]+MLUTIL_VERSION[\t ]+\"([^\"]*)\".*" "\\1" MLUTIL_VERSION "${_MLUTIL_VERSION_DEFINE}")
  unset(_MLUTIL_VERSION_DEFINE)
endif()

set(MLUTIL_DEFINITIONS)
set(MLUTIL_INCLUDE_DIRS ${MLUTIL_INCLUDE_DIR})
set(MLUTIL_LIBRARIES ${MLUTIL_LIBRARY})

find_package_handle_standard_args(
  MLUTIL
  FOUND_VAR MLUTIL_FOUND
  REQUIRED_VARS MLUTIL_INCLUDE_DIR MLUTIL_LIBRARY
  VERSION_VAR MLUTIL_VERSION)

if(MLUTIL_FOUND AND NOT TARGET MlUtil::MlUtil)
  add_library(MlUtil::MlUtil UNKNOWN IMPORTED)
  if(MLUTIL_LIBRARY_RELEASE)
    set_property(TARGET MlUtil::MlUtil APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
    set_target_properties(MlUtil::MlUtil PROPERTIES IMPORTED_LOCATION_RELEASE "${MLUTIL_LIBRARY_RELEASE}")
  endif()
  if(MLUTIL_LIBRARY_DEBUG)
    set_property(TARGET MlUtil::MlUtil APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
    set_target_properties(MlUtil::MlUtil PROPERTIES IMPORTED_LOCATION_DEBUG "${MLUTIL_LIBRARY_DEBUG}")
  endif()
  set_target_properties(MlUtil::MlUtil PROPERTIES INTERFACE_COMPILE_DEFINITIONS "COIN_DLL")
  set_target_properties(MlUtil::MlUtil PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${MLUTIL_INCLUDE_DIRS}")
endif()

mark_as_advanced(MLUTIL_DEFINITIONS)
mark_as_advanced(MLUTIL_INCLUDE_DIRS)
mark_as_advanced(MLUTIL_LIBRARIES)
