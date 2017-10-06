include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_POSITION_INDEPENDENT_CODE 1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable -Wno-sign-compare")

file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)

LEMON(Parser PARSER_SOURCE)
RAGEL(Lexer ${PARSER_SOURCE} SOURCES)

add_library(ace_tree OBJECT ${SOURCES})
target_compile_features(ace_tree PRIVATE cxx_nullptr)
