# Find the native TCLAP includes
#
#  TCLAP_INCLUDE_DIR - where to find CmdLine.h, etc.
#  TCLAP_FOUND       - True if TCLAP found.

find_path(TCLAP_INCLUDE_DIR
  NAMES tclap/CmdLine.h
  NO_DEFAULT_PATH
  PATHS
  /usr/include
  /usr/local/include
  $ENV{TCLAP_ROOT}/include
  ${TCLAP_ROOT}/include)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(TCLAP DEFAULT_MSG TCLAP_INCLUDE_DIR)
mark_as_advanced(TCLAP_INCLUDE_DIR)
