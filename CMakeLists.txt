#
# Project definition
#

project("ACE" C CXX)
cmake_minimum_required(VERSION 3.3)

include(cmake/GetGitRevisionDescription.cmake)
include(cmake/Lemon.cmake)
include(cmake/Ragel.cmake)

#
# Uninstall target
#

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake)

#
# Get the current GIT version
#

git_describe(VERSION)

string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" VERSION_SHA1 "${VERSION}")

set(LIB_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})
set(LIB_SOVERSION ${VERSION_MAJOR})

message(STATUS "Build version: " ${LIB_VERSION})

#
# Options
#

option(ACE_PLUGIN_LUA     "Enable LUA configuration plugin"    ON)
option(ACE_PLUGIN_PYTHON  "Enable PYTHON configuration plugin" ON)
option(ACE_PLUGIN_TOML    "Enable TOML configuration plugin"   ON)
option(ACE_PLUGIN_INI     "Enable INI configuration plugin"    ON)
option(ACE_PLUGIN_SEXP    "Enable SEXP configuration plugin"   ON)
option(ACE_PLUGIN_YAML    "Enable YAML configuration plugin"   ON)
option(ACE_BUILD_TESTS    "Build the Google tests suite"       OFF)

#
# Global definitions
#

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release ... FORCE)
endif()

add_definitions(-DACE_VERSION="${LIB_VERSION}")

#
# Global include directory
#

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#
# Flags preferences
#

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wno-unused-parameter")

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O3")

#
# Conan package integration
#

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup(KEEP_RPATHS)
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif()

#
# Dependencies checks
#

find_package(Threads REQUIRED)
find_package(TCLAP REQUIRED)
find_package(RE2 REQUIRED)
find_package(Lemon REQUIRED)
find_package(Ragel REQUIRED)

include_directories(${TCLAP_INCLUDE_DIR})
include_directories(${RE2_INCLUDE_DIR})

find_package(JANSSON REQUIRED)

if(ACE_PLUGIN_LUA)
  find_package(Lua52 REQUIRED)
endif()

if(ACE_PLUGIN_PYTHON)
  find_package(PythonLibs 2.7 REQUIRED)
endif()

if(ACE_PLUGIN_YAML)
  find_package(YamlCpp REQUIRED)
endif()

if(ACE_BUILD_TESTS)
  find_package(GTest REQUIRED)
endif()

#
# Subdirectories
#

add_subdirectory(libace)
add_subdirectory(tools)
add_subdirectory(formats/json)

if(ACE_PLUGIN_LUA OR ACE_BUILD_TESTS)
  add_subdirectory(formats/lua)
endif()

if(ACE_PLUGIN_PYTHON OR ACE_BUILD_TESTS)
  add_subdirectory(formats/python)
endif()

if(ACE_PLUGIN_TOML OR ACE_BUILD_TESTS)
  add_subdirectory(formats/toml)
endif()

if(ACE_PLUGIN_INI OR ACE_BUILD_TESTS)
  add_subdirectory(formats/ini)
endif()

if(ACE_PLUGIN_SEXP OR ACE_BUILD_TESTS)
  add_subdirectory(formats/sexp)
endif()

if(ACE_PLUGIN_YAML OR ACE_BUILD_TESTS)
  add_subdirectory(formats/yaml)
endif()

if(ACE_BUILD_TESTS)
  add_subdirectory(tests/libace)
  add_subdirectory(tests/codegen)
  enable_testing()
  #
  # Libace Google Tests
  #
  add_test(
    NAME libace
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests/libace
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libace-tests)
  set_property(
    TEST libace
    PROPERTY ENVIRONMENT
    "ACE_SCANNER_PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    "ACE_TESTS_PATH=${CMAKE_BINARY_DIR}/tests/libace")
  #
  # Generated code tests
  #
  add_test(
    NAME codegen
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests/codegen
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/codegen-tests)
  #
  set_property(
    TEST codegen
    PROPERTY ENVIRONMENT
    "ACE_SCANNER_PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    "ACE_TESTS_PATH=${CMAKE_BINARY_DIR}/tests/codegen")
endif()

#
# Header install rule
#

install(
  DIRECTORY   include/ace
  DESTINATION include)
