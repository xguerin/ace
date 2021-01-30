include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_POSITION_INDEPENDENT_CODE 1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable -Wno-sign-compare")

file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)

LEMON(Parser PARSER_SOURCE)
RAGEL(Lexer ${PARSER_SOURCE} SOURCES)

add_library(ace_ini_format SHARED ${SOURCES})

target_compile_features(ace_ini_format PRIVATE cxx_nullptr)
target_link_libraries(ace_ini_format PRIVATE ace)

set_target_properties(ace_ini_format
  PROPERTIES
  VERSION ${LIB_VERSION}
  SOVERSION ${LIB_SOVERSION})

install(
  TARGETS ace_ini_format
  LIBRARY DESTINATION lib)
