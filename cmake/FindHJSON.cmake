# Find the native HJSON includes and library

find_path(HJSON_INCLUDE_DIR
  NAMES hjson/hjson.h
  PATHS
  /usr/local
  /usr
  PATH_SUFFIXES include
  HINTS
  $ENV{HJSON_ROOT}
  ${HJSON_ROOT})

find_library(HJSON_LIBRARY
  NAMES hjson
  PATHS
  /usr/local
  /usr
  PATH_SUFFIXES lib
  HINTS
  $ENV{HJSON_ROOT}
  ${HJSON_ROOT})

FIND_PACKAGE_HANDLE_STANDARD_ARGS(HJSON DEFAULT_MSG HJSON_INCLUDE_DIR HJSON_LIBRARY)
mark_as_advanced(HJSON_INCLUDE_DIR HJSON_LIBRARY)
