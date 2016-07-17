# - Find ragel executable and provides macros to generate custom build rules
# The module defines the following variables
#
#  RAGEL_EXECUTABLE - path to the ragel program

find_program(RAGEL_EXECUTABLE ragel DOC "path to the ragel executable")
mark_as_advanced(RAGEL_EXECUTABLE)

if (NOT RAGEL_EXECUTABLE)
  message(FATAL_ERROR "Could NOT find the ragel executable")
endif ()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(RAGEL DEFAULT_MSG RAGEL_EXECUTABLE)
