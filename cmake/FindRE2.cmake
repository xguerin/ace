# Find the native RE2 includes and library

find_path(RE2_INCLUDE_DIR
  NAMES re2/re2.h
  PATHS
  /usr/local
  /usr
  PATH_SUFFIXES include
  HINTS
  $ENV{RE2_ROOT}
  ${RE2_ROOT})

find_library(RE2_LIBRARY
  NAMES re2
  PATHS
  /usr
  /usr/local
  PATH_SUFFIXES lib
  HINTS
  $ENV{RE2_ROOT}
  ${RE2_ROOT})

FIND_PACKAGE_HANDLE_STANDARD_ARGS(RE2 DEFAULT_MSG RE2_INCLUDE_DIR RE2_LIBRARY)
mark_as_advanced(RE2_LIBRARY RE2_INCLUDE_DIR)
