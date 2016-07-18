# - Find JANSSON 
# Find the native JANSSON includes and library
#
#  JANSSON_INCLUDE_DIRS - where to find re2.h, etc.
#  JANSSON_LIBRARIES    - List of libraries when using JANSSON.
#  JANSSON_FOUND        - True if JANSSON found.

if (JANSSON_INCLUDE_DIRS AND JANSSON_LIBRARIES)
  set(JANSSON_FOUND TRUE)
else ()
  find_path(JANSSON_INCLUDE_DIR NAMES jansson.h PATHS
    /usr/include
    /usr/local/include)
  find_library(JANSSON_LIBRARY NAMES jansson PATHS
    /usr/lib
    /usr/local/lib)
  if (JANSSON_INCLUDE_DIR AND JANSSON_LIBRARY)
    set(JANSSON_FOUND TRUE)
    set(JANSSON_INCLUDE_DIRS ${JANSSON_INCLUDE_DIR})
    set(JANSSON_LIBRARIES ${JANSSON_LIBRARY})
  else ()
    set(JANSSON_FOUND FALSE)
    set(JANSSON_LIBRARIES)
  endif ()
endif ()

if (JANSSON_FOUND)
  message(STATUS "Found JANSSON: ${JANSSON_LIBRARIES}")
else ()
  message(STATUS "Not Found JANSSON: ${JANSSON_LIBRARIES}")
  message(FATAL_ERROR "Could NOT find JANSSON library")
endif ()

mark_as_advanced(JANSSON_INCLUDE_DIRS JANSSON_LIBRARIES)
