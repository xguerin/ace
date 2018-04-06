# Locate yaml-cpp
#
# This module defines
#  YAMLCPP_FOUND, if false, do not try to link to yaml-cpp
#  YAMLCPP_LIBRARY, where to find yaml-cpp
#  YAMLCPP_INCLUDE_DIR, where to find yaml.h

find_path(YAMLCPP_INCLUDE_DIR yaml-cpp/yaml.h
  NO_DEFAULT_PATH
  PATHS
  /usr/local/include
  /usr/include
  $ENV{YAMLCPP_ROOT}/include
  ${YAMLCPP_ROOT}/include)

find_library(YAMLCPP_LIBRARY
  NAMES yaml-cpp
  NO_DEFAULT_PATH
  PATHS 
  /usr/local/lib
  /usr/lib
  $ENV{YAMLCPP_ROOT}/lib
  ${YAMLCPP_ROOT}/lib)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(YAMLCPP DEFAULT_MSG YAMLCPP_INCLUDE_DIR YAMLCPP_LIBRARY)
mark_as_advanced(YAMLCPP_INCLUDE_DIR YAMLCPP_LIBRARY)
