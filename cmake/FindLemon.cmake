#
# - Find lemon executable and provides macros to generate custom build rules
# The module defines the following variables
#
#  LEMON_EXECUTABLE - path to the lemon program
#  LEMON_TEMPLATE - location of the lemon template file

find_program(LEMON_EXECUTABLE lemon
  DOC "path to the lemon executable"
  NO_DEFAULT_PATH
  PATHS
  /usr/local/bin
  /usr/bin
  $ENV{LEMON_ROOT}/bin
  ${LEMON_ROOT}/bin)

if(LEMON_EXECUTABLE AND NOT LEMON_TEMPLATE)
	get_filename_component(LEMON_PATH ${LEMON_EXECUTABLE} PATH)
	if(LEMON_PATH)
    set(LEMON_TEMPLATE ${LEMON_PATH}/../share/lemon/lempar.c)
	endif(LEMON_PATH)
endif(LEMON_EXECUTABLE AND NOT LEMON_TEMPLATE)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(LEMON DEFAULT_MSG LEMON_EXECUTABLE LEMON_TEMPLATE)
mark_as_advanced(LEMON_EXECUTABLE LEMON_TEMPLATE)
