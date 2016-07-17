#
# - Find lemon executable and provides macros to generate custom build rules
# The module defines the following variables
#
#  LEMON_EXECUTABLE - path to the lemon program
#  LEMON_TEMPLATE - location of the lemon template file

find_program(LEMON_EXECUTABLE lemon DOC "path to the lemon executable")
mark_as_advanced(LEMON_EXECUTABLE)

if (NOT LEMON_EXECUTABLE)
  message(FATAL_ERROR "Could NOT find the lemon executable")
endif ()

if(LEMON_EXECUTABLE AND NOT LEMON_TEMPLATE)
	get_filename_component(lemon_path ${LEMON_EXECUTABLE} PATH)
	if(lemon_path)
    set(LEMON_TEMPLATE ${lemon_path}/../share/lemon/lempar.c)
	endif(lemon_path)
endif(LEMON_EXECUTABLE AND NOT LEMON_TEMPLATE)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LEMON DEFAULT_MSG LEMON_EXECUTABLE LEMON_TEMPLATE)
mark_as_advanced(LEMON_TEMPLATE)
