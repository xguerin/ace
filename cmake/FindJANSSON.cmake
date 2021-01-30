# Find the native JANSSON includes and library

find_path(JANSSON_INCLUDE_DIR
  NAMES jansson.h
  PATHS
  /usr/local
  /usr
  PATH_SUFFIXES include
  HINTS
  $ENV{JANSSON_ROOT}
  ${JANSSON_ROOT})

find_library(JANSSON_LIBRARY
  NAMES jansson
  PATHS
  /usr/local
  /usr
  PATH_SUFFIXES lib
  HINTS
  $ENV{JANSSON_ROOT}
  ${JANSSON_ROOT})

FIND_PACKAGE_HANDLE_STANDARD_ARGS(JANSSON DEFAULT_MSG JANSSON_INCLUDE_DIR JANSSON_LIBRARY)
mark_as_advanced(JANSSON_INCLUDE_DIR JANSSON_LIBRARY)