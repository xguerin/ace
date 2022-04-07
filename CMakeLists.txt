cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0074 NEW)

#
# Includes
#

include(cmake/GetGitRevisionDescription.cmake)
include(cmake/Lemon.cmake)
include(cmake/Ragel.cmake)

#
# Compiler configuration
#

set(CMAKE_C_COMPILER_NAMES clang gcc)
set(CMAKE_CXX_COMPILER_NAMES clang++ g++)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#
# Project definition
#

project("ACE" VERSION 2.0.2 LANGUAGES C CXX)

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

option(ACE_PLUGIN_HJSON   "Enable HJSON configuration plugin"    ON)
option(ACE_PLUGIN_INI     "Enable INI configuration plugin"      ON)
option(ACE_PLUGIN_LUA     "Enable LUA configuration plugin"      ON)
option(ACE_PLUGIN_PYTHON  "Enable PYTHON configuration plugin"   ON)
option(ACE_PLUGIN_SEXP    "Enable SEXP configuration plugin"     ON)
option(ACE_PLUGIN_TOML    "Enable TOML configuration plugin"     ON)
option(ACE_PLUGIN_YAML    "Enable YAML configuration plugin"     ON)

option(ACE_PTHREADS_NP    "Use pthreads non-portable API"        ON)
option(ACE_BUILD_TESTS    "Build the Google tests suite"         OFF)

option(ACE_ENABLE_ASAN    "Enable address sanitizer"             OFF)
option(ACE_ENABLE_MSAN    "Enable memory sanitizer"              OFF)
option(ACE_ENABLE_UBSAN   "Enable undefined behavior sanitizer"  OFF)

#
# Global definitions
#

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_definitions(-DACE_VERSION="${LIB_VERSION}")

if(ACE_PTHREADS_NP)
  add_definitions(-DACE_PTHREADS_NP)
endif()

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

if(ACE_ENABLE_ASAN OR ACE_ENABLE_MSAN OR ACE_ENABLE_UBSAN)
  if(NOT ${CMAKE_BUILD_TYPE} MATCHES "Debug")
    message(FATAL_ERROR "Sanitizers can only be enabled in Debug mode")
  endif()

  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer")
  add_link_options(-fno-omit-frame-pointer)

  if(ACE_ENABLE_ASAN)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
    add_link_options(-fsanitize=undefined)
  endif()

  if(ACE_ENABLE_MSAN)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=memory")
    add_link_options(-fsanitize=undefined)
  endif()

  if(ACE_ENABLE_UBSAN)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined")
    add_link_options(-fsanitize=undefined)
  endif()
endif()

#
# Dependencies checks
#

find_package(ClangFormat)
find_package(ClangTidy)

find_package(Threads REQUIRED)
find_package(TCLAP REQUIRED)
find_package(RE2 REQUIRED)
find_package(Lemon REQUIRED)
find_package(Ragel REQUIRED)

include_directories(${TCLAP_INCLUDE_DIR})
include_directories(${RE2_INCLUDE_DIR})

find_package(JANSSON REQUIRED)

if(ACE_PLUGIN_HJSON)
  find_package(HJSON REQUIRED)
endif()

if(ACE_PLUGIN_LUA)
  find_package(Lua52 REQUIRED)
endif()

if(ACE_PLUGIN_PYTHON)
  find_package(Python3 COMPONENTS Development REQUIRED)
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

if(ACE_PLUGIN_HJSON)
  add_subdirectory(formats/hjson)
endif()

if(ACE_PLUGIN_INI)
  add_subdirectory(formats/ini)
endif()

if(ACE_PLUGIN_LUA OR ACE_BUILD_TESTS)
  add_subdirectory(formats/lua)
endif()

if(ACE_PLUGIN_PYTHON)
  add_subdirectory(formats/python)
endif()

if(ACE_PLUGIN_SEXP)
  add_subdirectory(formats/sexp)
endif()

if(ACE_PLUGIN_TOML)
  add_subdirectory(formats/toml)
endif()

if(ACE_PLUGIN_YAML)
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
    "ACE_TESTS_PATH=${CMAKE_BINARY_DIR}/tests/libace"
    "LSAN_OPTIONS=suppressions=${CMAKE_SOURCE_DIR}/misc/suppressions.txt")
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

#
# Formatting
#

file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR}
  formats/*.h formats/*.cpp
  include/*.h
  libace/*.h libace/*.cpp
  tests/*.h tests/*.cpp
  tools/*.h tools/*.cpp)

foreach (SOURCE ${SOURCES})
  string(REGEX REPLACE "[/.]" "_" TARGET ${SOURCE})
  string(TOLOWER ${TARGET} TARGET)
  #
  if (ClangFormat_FOUND)
    add_custom_target(${TARGET}_format COMMAND clang-format -i -style=file ${CMAKE_SOURCE_DIR}/${SOURCE})
    list(APPEND FORMAT_TARGETS ${TARGET}_format)
  endif ()
  #
  if (ClangTidy_FOUND)
    add_custom_target(${TARGET}_tidy COMMAND clang-tidy -fix -quiet -format-style=file -p ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/${SOURCE})
    list(APPEND TIDY_TARGETS ${TARGET}_tidy)
  endif ()
endforeach ()

if (ClangFormat_FOUND)
  add_custom_target(format DEPENDS ${FORMAT_TARGETS})
endif ()

if (ClangTidy_FOUND)
  add_custom_target(tidy DEPENDS ${TIDY_TARGETS})
endif ()
