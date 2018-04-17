# Locate yaml-cpp
#
# This module defines
#  YAMLCPP_FOUND, if false, do not try to link to yaml-cpp
#  YAMLCPP_LIBRARY, where to find yaml-cpp
#  YAMLCPP_INCLUDE_DIR, where to find yaml.h

find_path(YAMLCPP_INCLUDE_DIR
  NAMES yaml-cpp/yaml.h
  PATHS
  /usr/local
  /usr
  PATH_SUFFIXES include
  HINTS
  $ENV{YAMLCPP_ROOT}
  ${YAMLCPP_ROOT})

find_library(YAMLCPP_LIBRARY
  NAMES yaml-cpp
  PATHS 
  /usr/local
  /usr
  PATH_SUFFIXES lib
  HINTS
  $ENV{YAMLCPP_ROOT}
  ${YAMLCPP_ROOT})

FIND_PACKAGE_HANDLE_STANDARD_ARGS(YamlCpp DEFAULT_MSG YAMLCPP_INCLUDE_DIR YAMLCPP_LIBRARY)
mark_as_advanced(YAMLCPP_INCLUDE_DIR YAMLCPP_LIBRARY)
