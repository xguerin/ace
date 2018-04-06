# - Find ragel executable and provides macros to generate custom build rules
# The module defines the following variables
#
#  RAGEL_EXECUTABLE - path to the ragel program

find_program(RAGEL_EXECUTABLE ragel DOC "path to the ragel executable")

FIND_PACKAGE_HANDLE_STANDARD_ARGS(RAGEL DEFAULT_MSG RAGEL_EXECUTABLE)
mark_as_advanced(RAGEL_EXECUTABLE)
