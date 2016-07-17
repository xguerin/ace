# - Find JANSSON 
# Find the native JANSSON includes and library
#
#  JANSSON_INCLUDE_DIR - where to find re2.h, etc.
#  JANSSON_LIBRARIES   - List of libraries when using JANSSON.
#  JANSSON_FOUND       - True if JANSSON found.

find_path(JANSSON_INCLUDE_DIR NAMES jansson.h PATHS
  /usr/include
  /usr/local/include
  )

find_library(JANSSON_LIBRARY NAMES jansson PATHS
  /usr/lib
  /usr/local/lib
  )

if (JANSSON_INCLUDE_DIR AND JANSSON_LIBRARY)
  set(JANSSON_FOUND TRUE)
  set(JANSSON_LIBRARIES ${JANSSON_LIBRARY} )
else ()
  set(JANSSON_FOUND FALSE)
  set( JANSSON_LIBRARIES )
endif ()

if (JANSSON_FOUND)
  message(STATUS "Found JANSSON: ${JANSSON_LIBRARY}")
else ()
  message(STATUS "Not Found JANSSON: ${JANSSON_LIBRARY}")
  message(FATAL_ERROR "Could NOT find JANSSON library")
endif ()

mark_as_advanced(JANSSON_INCLUDE_DIRS JANSSON_LIBRARIES)
