# - Find TCLAP 
# Find the native TCLAP includes and library
#
#  TCLAP_INCLUDE_DIR - where to find re2.h, etc.
#  TCLAP_LIBRARIES   - List of libraries when using TCLAP.
#  TCLAP_FOUND       - True if TCLAP found.

find_path(TCLAP_INCLUDE_DIR NAMES tclap/CmdLine.h PATHS
  /usr/include
  /usr/local/include
  )

if (TCLAP_INCLUDE_DIR)
  set(TCLAP_FOUND TRUE)
else ()
  set(TCLAP_FOUND FALSE)
endif ()

if (TCLAP_FOUND)
  message(STATUS "Found TCLAP: ${TCLAP_INCLUDE_DIR}")
else ()
  message(STATUS "Not Found TCLAP: ${TCLAP_INCLUDE_DIR}")
  message(FATAL_ERROR "Could NOT find TCLAP library")
endif ()

mark_as_advanced(TCLAP_INCLUDE_DIRS)
