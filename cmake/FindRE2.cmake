# - Find RE2 
# Find the native RE2 includes and library
#
#  RE2_INCLUDE_DIR - where to find re2.h, etc.
#  RE2_LIBRARY     - List of libraries when using RE2.
#  RE2_FOUND       - True if RE2 found.

find_path(RE2_INCLUDE_DIR re2/re2.h
  NO_DEFAULT_PATH
  PATHS
  /usr/include
  /usr/local/include
  $ENV{RE2_ROOT}/include
  ${RE2_ROOT}/include)

find_library(RE2_LIBRARY
  NAMES re2
  NO_DEFAULT_PATH
  PATHS
  /usr/local/lib
  /usr/lib
  $ENV{RE2_ROOT}/lib
  ${RE2_ROOT}/lib)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(RE2 DEFAULT_MSG RE2_INCLUDE_DIR RE2_LIBRARY)
mark_as_advanced(RE2_LIBRARY RE2_INCLUDE_DIR)
