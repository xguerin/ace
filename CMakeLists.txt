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

set(VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
message(STATUS "Build version: " ${VERSION_SHORT})

#
# Options
#

option(ACE_PLUGIN_JSON    "Enable JSON configuration plugin"   ON)
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

add_definitions(-DACE_VERSION="${VERSION_SHORT}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#
# Flags preferences
#

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wno-unused-parameter")
else()
  set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wno-unused-parameter -pthread")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O3")

#
# RPATHs handling
#

set(CMAKE_SKIP_BUILD_RPATH            FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH    FALSE)
set(CMAKE_INSTALL_RPATH               "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif("${isSystemDir}" STREQUAL "-1")

#
# Dependencies checks
#

find_package(Threads REQUIRED)
find_package(TCLAP REQUIRED)
find_package(RE2 REQUIRED)
find_package(Lemon REQUIRED)
find_package(Ragel REQUIRED)

include_directories(${TCLAP_INCLUDE_DIRS})
include_directories(${RE2_INCLUDE_DIRS})
include_directories(${LEMON_INCLUDE_DIRS})
include_directories(${RAGEL_INCLUDE_DIRS})

if (ACE_PLUGIN_JSON)
  find_package(JANSSON REQUIRED)
endif ()

if (ACE_PLUGIN_LUA)
  find_package(Lua52 REQUIRED)
endif ()

if (ACE_PLUGIN_PYTHON)
  find_package(PythonLibs 2.7 REQUIRED)
endif ()

if (ACE_PLUGIN_YAML)
  find_package(YamlCpp REQUIRED)
endif ()

if (ACE_BUILD_TESTS)
  find_package(GTest REQUIRED)
endif ()

#
# Subdirectories
#

add_subdirectory(libace)
add_subdirectory(tools)

if (ACE_PLUGIN_JSON OR ACE_BUILD_TESTS)
  add_subdirectory(formats/json)
endif ()

if (ACE_PLUGIN_LUA OR ACE_BUILD_TESTS)
  add_subdirectory(formats/lua)
endif ()

if (ACE_PLUGIN_PYTHON OR ACE_BUILD_TESTS)
  add_subdirectory(formats/python)
endif ()

if (ACE_PLUGIN_TOML OR ACE_BUILD_TESTS)
  add_subdirectory(formats/toml)
endif ()

if (ACE_PLUGIN_INI OR ACE_BUILD_TESTS)
  add_subdirectory(formats/ini)
endif ()

if (ACE_PLUGIN_SEXP OR ACE_BUILD_TESTS)
  add_subdirectory(formats/sexp)
endif ()

if (ACE_PLUGIN_YAML OR ACE_BUILD_TESTS)
  add_subdirectory(formats/yaml)
endif ()

if (ACE_BUILD_TESTS)
  add_subdirectory(tests/libace)
  add_subdirectory(tests/codegen)
  enable_testing()
  #
  add_test(NAME libace WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/libace COMMAND libace-tests)
  set_property(TEST libace PROPERTY ENVIRONMENT
    "ACE_SCANNER_PATH=${CMAKE_CURRENT_BINARY_DIR}/formats/json:${CMAKE_CURRENT_BINARY_DIR}/formats/lua:${CMAKE_CURRENT_BINARY_DIR}/formats/python:${CMAKE_CURRENT_BINARY_DIR}/formats/toml"
    "ACE_TESTS_PATH=${CMAKE_CURRENT_BINARY_DIR}/tests/libace"
    )
  #
  add_test(NAME codegen WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/codegen COMMAND codegen-tests)
  set_property(TEST codegen PROPERTY ENVIRONMENT
    "ACE_SCANNER_PATH=${CMAKE_CURRENT_BINARY_DIR}/formats/json:${CMAKE_CURRENT_BINARY_DIR}/formats/lua:${CMAKE_CURRENT_BINARY_DIR}/formats/python:${CMAKE_CURRENT_BINARY_DIR}/formats/toml"
    "ACE_TESTS_PATH=${CMAKE_CURRENT_BINARY_DIR}/tests/codegen"
    )
endif ()

#
# Header install rule
#

install(
  DIRECTORY   include/ace
  DESTINATION include 
  )
