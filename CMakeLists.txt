#
# Project definition
#

project("ACE" C CXX)
cmake_minimum_required(VERSION 3.3)

include(cmake/Lemon.cmake)
include(cmake/Ragel.cmake)

#
# Options
#

option(ACE_PLUGIN_JSON    "Enable JSON configuration plugin"    ON)
option(ACE_PLUGIN_LUA     "Enable LUA configuration plugin"     ON)
option(ACE_PLUGIN_PYTHON  "Enable PYTHON configuration plugin"  ON)
option(ACE_BUILD_TESTS    "Build the Google tests suite"        OFF)

#
# Global definitions
#

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_BUILD_TYPE Release)

add_definitions(-DACE_VERSION="1.0.0")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#
# Flags preferences
#

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wno-unused-parameter -pthread")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O3")

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
  find_package(Lua51 REQUIRED)
endif ()

if (ACE_PLUGIN_PYTHON)
  find_package(PythonLibs 2.7 REQUIRED)
endif ()

if (ACE_BUILD_TESTS)
  find_package(GTest REQUIRED)
endif ()

#
# Subdirectories
#

add_subdirectory(libace)
add_subdirectory(tools)

if (ACE_PLUGIN_JSON)
  add_subdirectory(formats/json)
endif ()

if (ACE_PLUGIN_LUA)
  add_subdirectory(formats/lua)
endif ()

if (ACE_PLUGIN_PYTHON)
  add_subdirectory(formats/python)
endif ()

if (ACE_BUILD_TESTS)
  add_subdirectory(tests)
endif ()

#
# Header install rule
#

install(
  DIRECTORY   include/ace
  DESTINATION include 
  )
