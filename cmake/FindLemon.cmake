# Find lemon executable and provides macros to generate custom build rules

find_program(LEMON_EXECUTABLE
  NAMES lemon
  DOC "path to the lemon executable"
  PATHS
  /usr/local
  /usr
  PATH_SUFFIXES bin
  HINTS
  $ENV{LEMON_ROOT}
  ${LEMON_ROOT})

if(LEMON_EXECUTABLE AND NOT LEMON_TEMPLATE)
  get_filename_component(LEMON_PATH ${LEMON_EXECUTABLE} PATH)
	if(LEMON_PATH)
    find_file(LEMON_TEMPLATE
      NAMES
      lempar.c
      PATHS
      ${LEMON_PATH}/../share/lemon)
	endif(LEMON_PATH)
endif(LEMON_EXECUTABLE AND NOT LEMON_TEMPLATE)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Lemon REQUIRED_VARS LEMON_EXECUTABLE LEMON_TEMPLATE)
mark_as_advanced(LEMON_EXECUTABLE LEMON_TEMPLATE)
